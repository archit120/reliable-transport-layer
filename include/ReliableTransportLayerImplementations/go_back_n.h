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

//packet structure 
/* 
 <4 bytes CRC-32><<1-bit ACK>
 <7-bit sequence number>>
 <message data>
*/
class GoBackN : ReliableTransportLayer {

    public:
        GoBackN(int N);
        int send(const void *msg, int len); 
        int recv(void *buf, int len);
};

#endif