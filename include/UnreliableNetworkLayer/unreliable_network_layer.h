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

using namespace std;

#define DEBUG

/*
 * UnreliableNetworkLayer implements the network layer
 * It allows bidirectional communication between two hosts identified by id
 * The id can be 0/1
 *
 * The constructor parameters give full control over the "unreliability"
 * The maximum size of message this layer can send is MAX_PACKET_SIZE
 *
 * TODO: Implement bandwith control
 * TODO: Implement some way to notify listeners
 */
class UnreliableNetworkLayer {
    private: 
        double _prob_loss;
        double _prob_corrupt; 
        int _expected_delay;
        double _bandwith;

        int partial_read = 0;
        default_random_engine generator;
        bernoulli_distribution loss_distribution;
        bernoulli_distribution corrupt_distribution;
        gamma_distribution<double> delay_distribution;

        queue<pair<shared_ptr<uint8_t>, int>> message_queue[2];
        condition_variable listener_cv[2];
        mutex m[2];

    public:
        UnreliableNetworkLayer(double prob_loss=0, double prob_corrupt=0, int expected_delay=1, double bandwidth = 0);

        int send(const void *msg, int len, int id); 

        int recv(void *buf, int len, int id);

    int notify(int id);
    int fake_notify(int id);
};

#endif