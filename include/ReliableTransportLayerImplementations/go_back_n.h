#ifndef GBN_H
#define GBN_H

#include <random>
#include <memory>
#include <chrono>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <cstring>

#include "ReliableTransportLayer/reliable_transport_layer.h"

using namespace std;

class GoBackN : ReliableTransportLayer {

    public:
        GoBackN(int N);
        int send(const void *msg, int len); 
        int recv(void *buf, int len);
};

#endif