#ifndef UNRELIABLE_NETWORK_LAYER_H
#define UNRELIABLE_NETWORK_LAYER_H

#include <random>
#include <memory>
#include <chrono>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <cstring>

using namespace std;

class UnreliableNetworkLayer {
    private: 
        double _prob_loss;
        double _prob_corrupt; 
        int _expected_delay;
        double _bandwith;
        
        default_random_engine generator;
        bernoulli_distribution loss_distribution;
        bernoulli_distribution corrupt_distribution;
        geometric_distribution<int> delay_distribution;

        queue<pair<shared_ptr<uint8_t>, int>> message_queue;
        mutex m;

    public:
        UnreliableNetworkLayer(double prob_loss=0, double prob_corrupt=0, int expected_delay=1, double bandwidth = 0);

        int send(const void *msg, int len); 

        int recv(void *buf, int len);

};

#endif