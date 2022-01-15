#include "UnreliableNetworkLayer/unreliable_network_layer.h"
#include <iostream>

// packet is lost with prob_loss, all bytes are independetly corrupted by prob_correct, a packet takes a geometric time with expectation expected_delay to reach
UnreliableNetworkLayer::UnreliableNetworkLayer(double prob_loss, double prob_corrupt, int expected_delay)
{
    _prob_loss = prob_loss, _prob_corrupt = prob_corrupt, _expected_delay = expected_delay;

    generator = default_random_engine(time(0));

    loss_distribution = bernoulli_distribution(prob_loss);
    corrupt_distribution = bernoulli_distribution(prob_corrupt);
    delay_distribution = geometric_distribution<int>(1.0 / expected_delay);
}

//send data into unreliable channel
int UnreliableNetworkLayer::send(const void *msg, int len)
{
    if (loss_distribution(generator))
        return len;

    shared_ptr<uint8_t> temp_buffer(new uint8_t[len]);
    memcpy((void *)temp_buffer.get(), msg, len);
    for (int i = 0; i < len; i++)
        if(corrupt_distribution(generator))
            temp_buffer.get()[i] = (uint8_t)rand();
    
    if(_expected_delay==1) {
        lock_guard<mutex> lg(m);
        message_queue.push(make_pair(temp_buffer, len));
        return len;
    }

    int delay = delay_distribution(generator)+1;

    // no timers in Cpp so create a new thread and detach
    
    thread timerthread([delay, temp_buffer, len, this]() {
        this_thread::sleep_for(chrono::milliseconds(delay));
        lock_guard<mutex> lg(m);
        message_queue.push(make_pair(temp_buffer, len));
    });

    timerthread.detach();

    return len;
}

//receive data from unreliable channel
int UnreliableNetworkLayer::recv(void *buf, int len)
{
    lock_guard<mutex> lg(m);
    if(message_queue.size()==0) return 0;
    auto top = message_queue.front();
    message_queue.pop();
    int alen = min(len, top.second);
    memcpy(buf, top.first.get(), alen);
    return alen;
}
