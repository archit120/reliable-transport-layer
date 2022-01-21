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
#include "helpers/general.h"

using namespace std;

#define MAX_MESSAGE_SIZE (MAX_PACKET_SIZE-9)
#define MAX_TIMEOUT_MS 15

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
    atomic<uint32_t> base_n;
    uint32_t current_n;
    deque<tuple<shared_ptr<uint8_t>, uint32_t, chrono::steady_clock::time_point, int>> buffered_packets;
    void notifierFunc();
    void handlePacket();
    void timerFunc();
    thread notifier;
    thread timer;
    shared_ptr<bool> isThreadAlive;
    unique_ptr<Semaphore> semaphore;
    mutex bufferLock;
    public:
        GoBackNSender(int N, shared_ptr<UnreliableNetworkLayer> unreliable_network_layer);

     ~GoBackNSender();

public:
    int send(const void *msg, int len);

};

class GoBackNReceiver: ReliableTransportLayerReceiver {
    queue<uint8_t> bytebuffer;
    uint32_t current_n;
    void notifierFunc();
    void handlePacket();
    thread notifier;
    shared_ptr<bool> isThreadAlive;
public:
    GoBackNReceiver(shared_ptr<UnreliableNetworkLayer> unreliable_network_layer);

    ~GoBackNReceiver();
    int recv(char *msg, int len);

};

#endif