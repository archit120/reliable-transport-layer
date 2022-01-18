#include "UnreliableNetworkLayer/unreliable_network_layer.h"
#include <iostream>

// TODO: implement bandwidth limits
// packet is lost with prob_loss, all bytes are independetly corrupted by prob_correct, a packet takes a geometric time with expectation expected_delay to reach
UnreliableNetworkLayer::UnreliableNetworkLayer(double prob_loss, double prob_corrupt, int expected_delay, double bandwith)
{
    _prob_loss = prob_loss, _prob_corrupt = prob_corrupt, _expected_delay = expected_delay;

    generator = default_random_engine(time(0));

    loss_distribution = bernoulli_distribution(prob_loss);
    corrupt_distribution = bernoulli_distribution(prob_corrupt);

    if (_expected_delay != 1)
        delay_distribution = gamma_distribution<double>(7, expected_delay/7.0);
}

//send data into unreliable channel, id is 0/1
int UnreliableNetworkLayer::send(const void *msg, int len, int id)
{
    if (loss_distribution(generator))
        return len;

    shared_ptr<uint8_t> temp_buffer(new uint8_t[len]);
    memcpy((void *)temp_buffer.get(), msg, len);
    for (int i = 0; i < len; i++)
        if (corrupt_distribution(generator))
            temp_buffer.get()[i] = (uint8_t)rand();

    if (_expected_delay == 1)
    {
        lock_guard<mutex> lg(m);
        message_queue[id].push(make_pair(temp_buffer, len));
        return len;
    }
    int delay = (int)(delay_distribution(generator) + 1);
    // cout << delay << "\n";
    // no timers in Cpp so create a new thread and detach

    thread timerthread([delay, temp_buffer, len, id, this]()
                       {
                           this_thread::sleep_for(chrono::microseconds(delay));
                           lock_guard<mutex> lg(m);
                           message_queue[id].push(make_pair(temp_buffer, len));
                       });

    timerthread.detach();

    return len;
}

//receive data from unreliable channel, id is 0/1
int UnreliableNetworkLayer::recv(void *buf, int len, int id)
{
    lock_guard<mutex> lg(m);
    if (message_queue[!id].size() == 0)
        return 0;
    auto top = message_queue[!id].front();
    message_queue[!id].pop();
    int alen = min(len, top.second);
    memcpy(buf, top.first.get(), alen);
    return alen;
}
