#include "UnreliableNetworkLayer/unreliable_network_layer.h"
#include <iostream>
#include <helpers/general.h>

// TODO: implement bandwidth limits
// packet is lost with prob_loss, all bytes are independently corrupted by prob_correct, a packet takes a geometric time with expectation expected_delay to reach
UnreliableNetworkLayer::UnreliableNetworkLayer(double prob_loss, double prob_corrupt, int expected_delay, double bandwith)
{
    _prob_loss = prob_loss, _prob_corrupt = prob_corrupt, _expected_delay = expected_delay;

    generator = default_random_engine(time(0));

    loss_distribution = bernoulli_distribution(prob_loss);
    corrupt_distribution = bernoulli_distribution(prob_corrupt);
    is_dying = false;
    if (_expected_delay != 1) {
        delay_distribution = gamma_distribution<double>(7, expected_delay / 7.0);
        timer = thread([this]() {
           while(!is_dying) {
               clock++;
               for(int i = 0;i<2;i++) {
                   lock_guard<mutex> lg(m[i]);
                   while(message_timer_queue[i].size() && get<0>(message_timer_queue[i].top()) >= clock) {
                       auto newp = message_timer_queue[i].top();
                       message_timer_queue[i].pop();
                       message_queue[i].push(make_pair(get<2>(newp), get<1>(newp)));
                       listener_cv[i].notify_one();
                   }
                   this_thread::sleep_for(chrono::microseconds(1));
               }
           }
        });
    }
}

// send data into unreliable channel, id is 0/1
// reject packets larger than MAX_PACKET_SIZE bytes
int UnreliableNetworkLayer::send(const void *msg, int len, int id)
{
    SPDLOG_TRACE("Sending packet of size {} from {}", len ,id);
    dumpPacket((const uint8_t *)msg, len);
    if(len > MAX_PACKET_SIZE)
        return 0;

    if (loss_distribution(generator))
        return len;

    shared_ptr<uint8_t> temp_buffer(new uint8_t[len]);
    memcpy((void *)temp_buffer.get(), msg, len);
    for (int i = 0; i < len; i++)
        if (corrupt_distribution(generator))
            temp_buffer.get()[i] = (uint8_t)rand();
    dumpPacket((const uint8_t *)temp_buffer.get(), len);

    if (_expected_delay == 1)
    {
        lock_guard<mutex> lg(m[!id]);
        message_queue[!id].push(make_pair(temp_buffer, len));
        listener_cv[!id].notify_one();
        return len;
    }
    int delay = (int)(delay_distribution(generator) + 1);
    // no timers in Cpp so create a new thread and detach

   lock_guard<mutex> lg(m[!id]);
    message_timer_queue[!id].push(make_tuple(delay+clock, len, temp_buffer));

    return len;
}

//receive data from unreliable channel, id is 0/1
int UnreliableNetworkLayer::recv(void *buf, int len, int id)
{

    lock_guard<mutex> lg(m[id]);
    if (message_queue[id].size() == 0)
        return 0;
    auto top = message_queue[id].front();
    int alen = 0;
    if(len >= top.second - partial_read) {
        message_queue[id].pop();
        alen = top.second - partial_read;
        memcpy(buf, top.first.get()+partial_read, top.second - partial_read);
        partial_read = 0;
    }
    else {
        alen = len;
        memcpy(buf, top.first.get()+partial_read, len);
        partial_read += len;
    }
    SPDLOG_TRACE("Receiving packet of size {} at {}", alen ,id);

    return alen;
}

// returns only when a new packet arrives
int UnreliableNetworkLayer::notify(int id)
{
    SPDLOG_TRACE("Notifier for {} locked", id);

    unique_lock<mutex> lg(m[id]);
    while (message_queue[id].size() == 0)
        listener_cv[id].wait(lg);
    SPDLOG_TRACE("Notifier for {} unlocked", id);

    return 1;
}

int UnreliableNetworkLayer::fake_notify(int id) {
    SPDLOG_DEBUG("Fake notify for {}", id);

    unique_lock<mutex> lg(m[id]);
    shared_ptr<uint8_t> fbuffer(0);
    message_queue[id].push(make_pair(fbuffer, 0));
    listener_cv[id].notify_all();
    return 0;
}

UnreliableNetworkLayer::~UnreliableNetworkLayer() {
    is_dying = true;
    if(_expected_delay != 1)
        timer.join();
}

