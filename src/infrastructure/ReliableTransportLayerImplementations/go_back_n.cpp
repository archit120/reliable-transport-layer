#include "ReliableTransportLayerImplementations/go_back_n.h"
#include "helpers/crc.h"

#define MAX_MESSAGE_SIZE (MAX_PACKET_SIZE-8)

bool checkPacket(const void* msg, int len) {
    uint32_t crc = crc32buf((const char *)msg+4, len-4);
    return crc == ((const uint32_t*)msg)[0];
}

void GoBackNSender::notifierFunc() {
    try {
//        Wait for something to happen
        while(_notify() && *isThreadAlive) {
//            Potential Race
            handlePacket();
        }
    }
    catch (exception& e) {
//        Catch all exception, ignore and die
    }
}


GoBackNSender::GoBackNSender(int N, shared_ptr<UnreliableNetworkLayer> unreliable_network_layer) : _N(N), ReliableTransportLayerSender(unreliable_network_layer) {
    notifier = thread(&GoBackNSender::notifierFunc, this);
    isThreadAlive = shared_ptr<bool>(new bool());
    *isThreadAlive = true;
}

GoBackNSender::~GoBackNSender() {
    *isThreadAlive = false;
    notifier.detach();
}

int GoBackNSender::send(const void *msg, int len) {
    if(len > MAX_MESSAGE_SIZE) {
        for(int i = 0;i<len; i+=MAX_MESSAGE_SIZE)
            send(msg, min(MAX_MESSAGE_SIZE, len-i));
        return len;
    }
    return ReliableTransportLayerSender::send(msg, len);
}

void GoBackNSender::handlePacket() {
    static char buffer[MAX_PACKET_SIZE];
    int len = _recv(buffer, MAX_PACKET_SIZE);
    if(!checkPacket(buffer, len))
        return;

//    Handle ACK

    uint32_t seqno = ((const uint32_t*)buffer)[1];
    if(!(seqno>>31))    return;

    seqno ^= 1<<31;

    base_n += seqno;
}


