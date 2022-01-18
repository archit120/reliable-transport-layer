#include "ReliableTransportLayerImplementations/go_back_n.h"
#include "helpers/crc.h"

bool checkPacket(const void* msg, int len) {
    uint32_t crc = crc32buf((const char *)msg+4, len-4);
    return crc == ((const uint32_t*)msg)[0];
}

// TODO: figure out a good way to destroy this thread.
void GoBackNSender::notifierFunc() {
    try {
//        Wait for something to happen
        while(_notify()) {
            handlePacket();
        }
    }
    catch (exception& e) {
//        Catch all exception, ignore and die
//        This will always happen when the class is destroyed
    }
}


GoBackNSender::GoBackNSender(int N, shared_ptr<UnreliableNetworkLayer> unreliable_network_layer) : _N(N), ReliableTransportLayerSender(unreliable_network_layer) {
    notifier = thread(&GoBackNSender::notifierFunc, this);
}

GoBackNSender::~GoBackNSender() {
    notifier.detach(); // this will cause that thread to crash
}

int GoBackNSender::send(const void *msg, int len) {
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


