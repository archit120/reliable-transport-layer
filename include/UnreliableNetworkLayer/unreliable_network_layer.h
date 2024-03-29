#ifndef UNRELIABLE_NETWORK_LAYER_H
#define UNRELIABLE_NETWORK_LAYER_H

#define MAX_PACKET_SIZE 1500

#include <random>
#include <memory>
#include <chrono>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <cstring>
#include <spdlog/spdlog.h>

using namespace std;

/*
 * UnreliableNetworkLayer implements the network layer
 * It allows bidirectional communication between two hosts identified by id
 * The id can be 0/1
 *
 * The constructor parameters give full control over the "unreliability"
 * The maximum size of message this layer can send is MAX_PACKET_SIZE
 *
 * TODO: Implement bandwith control
 */
class UnreliableNetworkLayer {
    private: 
        double _prob_loss;
        double _prob_corrupt; 
        int _expected_delay;
        bool is_dying;
        double _bandwith;

        int partial_read = 0;

        uint64_t clock = 0;
        thread timer;
        default_random_engine generator;
        bernoulli_distribution loss_distribution;
        bernoulli_distribution corrupt_distribution;
        gamma_distribution<double> delay_distribution;

        priority_queue<tuple<uint64_t, int, shared_ptr<uint8_t>>> message_timer_queue[2];
        queue<pair<shared_ptr<uint8_t>, int>> message_queue[2];
        condition_variable listener_cv[2];
        mutex m[2];

    public:
        UnreliableNetworkLayer(double prob_loss=0, double prob_corrupt=0, int expected_delay=1, double bandwidth = 0);

    virtual ~UnreliableNetworkLayer();

    int send(const void *msg, int len, int id);

        int recv(void *buf, int len, int id);

    int notify(int id);
    int fake_notify(int id);
};

#endif