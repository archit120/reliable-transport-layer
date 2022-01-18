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

 <4 bytes CRC-32>
 <<1-bit ACK><31-bit sequence number>>
 <message data>

 Total header size = 32 bytes
 Remaining packet size = MAX_PACKET_SIZE - 32

CRC-32 is calculated on the header+data.
ACKs are cumulative.

Sender maintains a timeout for each sent packet. If ACK is not received 
then it resends all packets between last received ACK for and upto n packets
ahead. Sender ignores value in ACK bit

Receiver does not buffer packets received out of order. 
 
*/
class GoBackNSender : ReliableTransportLayerSender {
    const int _N;
    uint32_t base_n;
    queue<pair<const void*, int>> buffered_packets;
    void notifierFunc();
    void handlePacket();
    thread notifier;
    shared_ptr<bool> isThreadAlive;
    public:
        GoBackNSender(int N, shared_ptr<UnreliableNetworkLayer> unreliable_network_layer);

     ~GoBackNSender();

private:
    int send(const void *msg, int len) override;

};

#endif