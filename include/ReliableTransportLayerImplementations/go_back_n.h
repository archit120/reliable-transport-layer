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
#include "helpers/semaphore.h"

using namespace std;

// 
/* 
packet structure

 <4 bytes CRC-32>
 <4-byte sequence number>
 <1-byte flag>
 <message data>

 Total header size = 32 bytes
 Remaining packet size = MAX_PACKET_SIZE - 32

 flag structure

 <7 bits unused><1 bit ACK>

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
    uint32_t current_n;
    queue<pair<shared_ptr<uint8_t>, int>> buffered_packets;
    void notifierFunc();
    void handlePacket();
    void timerFunc();
    thread notifier;
    thread timer;
    shared_ptr<bool> isThreadAlive;
    unique_ptr<Semaphore> semaphore;
    public:
        GoBackNSender(int N, shared_ptr<UnreliableNetworkLayer> unreliable_network_layer);

     ~GoBackNSender();

private:
    int send(const void *msg, int len) override;

};

#endif