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

// 
/* 
packet structure

 <4 bytes CRC-32><<1-bit ACK>
 <31-bit sequence number>>
 <message data>

Sender maintains a timeout for each sent packet. If ACK is not received 
then it resends all packets between last received ACK for and n packets
ahead. 
 
*/
class GoBackNSender : ReliableTransportLayerSender {

    public:
        GoBackNSender(int N);
        int send(const void *msg, int len); 
};

#endif