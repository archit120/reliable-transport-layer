#include "ReliableTransportLayerImplementations/go_back_n.h"
#include "helpers/crc.h"


bool checkPacket(const void* msg, int len) {
    if(len < 4) return false;
    uint32_t crc = crc32buf((const char *)msg+4, len-4);
    return crc == ((const uint32_t*)msg)[0];
}

void GoBackNSender::notifierFunc() {
    try {
//        Wait for something to happen
        shared_ptr<bool> localCopy = isThreadAlive;
        while(_notify() && *localCopy) {
//            Potential Race
            handlePacket();
        }
    }
    catch (exception& e) {
//        Catch all exception, ignore and die
    }
}


GoBackNSender::GoBackNSender(int N, shared_ptr<UnreliableNetworkLayer> unreliable_network_layer) : _N(N), ReliableTransportLayerSender(unreliable_network_layer) {
    isThreadAlive = shared_ptr<bool>(new bool());
    *isThreadAlive = true;
    notifier = thread(&GoBackNSender::notifierFunc, this);
    timer = thread(&GoBackNSender::timerFunc, this);

    semaphore.reset(new Semaphore(N));
    base_n = 0;
    current_n = 0;
}

GoBackNSender::~GoBackNSender() {
    *isThreadAlive = false;
    _fake_notify();
    timer.join();
    notifier.join();
}

// blocks if queue is full
int GoBackNSender::send(const void *msg, int len) {
    if(len > MAX_MESSAGE_SIZE) {
        for(int i = 0;i<len; i+=MAX_MESSAGE_SIZE)
            send((uint8_t *)msg+i, min(MAX_MESSAGE_SIZE, len-i));
        return len;
    }
    shared_ptr<uint8_t> buffer(new uint8_t[MAX_PACKET_SIZE], default_delete<uint8_t[]>());
    memcpy(buffer.get()+9, msg, len);
    buffer.get()[8] = 0;
    semaphore->down();
    ((uint32_t*)buffer.get())[1] = ++current_n;
    ((uint32_t*)buffer.get())[0] = crc32buf(reinterpret_cast<const char *>(buffer.get() + 4), len + 5) ;
    lock_guard<mutex> lg(bufferLock);

    buffered_packets.push_back(make_tuple(buffer, current_n, chrono::steady_clock::now(), len+9));
    return ReliableTransportLayerSender::_send(buffer.get(), len+9);
}

void GoBackNSender::handlePacket() {
    static char buffer[MAX_PACKET_SIZE];
    int len = _recv(buffer, MAX_PACKET_SIZE);
    if(!checkPacket(buffer, len))
        return;

//    Handle ACK

    uint32_t seqno = ((const uint32_t*)buffer)[1];
    uint8_t flags = ((const char*)buffer)[8];
    if(!(flags&1))    return;

    SPDLOG_DEBUG("Successfully handled ACK for {}", seqno);
    while(base_n < seqno)
        base_n++, semaphore->up();

}

void GoBackNSender::timerFunc() {
    try {
        shared_ptr<bool> localCopy = isThreadAlive;
        while (*localCopy) {
            this_thread::sleep_for(chrono::milliseconds(MAX_TIMEOUT_MS));
//            TODO: handle warping of seqno
            lock_guard<mutex> lg(bufferLock);
            while(buffered_packets.size() && get<1>(buffered_packets.front()) <= base_n) {
                SPDLOG_DEBUG("Removing buffered packet {0}", get<1>(buffered_packets.front()));
                buffered_packets.pop_front();
            }
            if(!buffered_packets.size())    continue;
            if(since(get<2>(buffered_packets.front())).count() < MAX_TIMEOUT_MS)
                continue;
            SPDLOG_DEBUG("Timer expired for packet {0}", get<1>(buffered_packets.front()));
            for(int i =0;i<buffered_packets.size();i++) {
//                time to resend all packets! :(
                auto buffered_packet = buffered_packets.front();
                buffered_packets.pop_front();
                ReliableTransportLayerSender::_send(get<0>(buffered_packet).get(), get<3>(buffered_packet));
                buffered_packets.push_back(make_tuple(get<0>(buffered_packet), get<1>(buffered_packet), chrono::steady_clock::now(), get<3>(buffered_packet)));
            }
        }
    }
    catch (exception& e) {

    }
}


void GoBackNReceiver::notifierFunc() {
    try {
//        Wait for something to happen
        shared_ptr<bool> localCopy = isThreadAlive;

    while(_notify() && *localCopy) {
//            Potential Race
        handlePacket();
    }
    }
    catch (exception& e) {
//        Catch all exception, ignore and die
    }

}

void GoBackNReceiver::handlePacket() {
    static char buffer[MAX_PACKET_SIZE];
    int len = _recv(buffer, MAX_PACKET_SIZE);
    if(!checkPacket(buffer, len))
        return;

//    Check seq

    uint32_t seqno = ((const uint32_t*)buffer)[1];
    if(seqno > current_n+1)
        return;
    if(current_n == seqno-1) {
        SPDLOG_DEBUG("Adding content of {} into buffer", seqno);
        dumpPacket((const uint8_t*)buffer+9, len-9);
        for (int i = 0; i < len - 9; i++)
            bytebuffer.push(buffer[i + 9]);
    }
    current_n = max(seqno, current_n);

//  Craft ACK
   static uint8_t ACKbuffer[9];
   ACKbuffer[8] = 1;
    ((uint32_t*)ACKbuffer)[1] = current_n;
    ((uint32_t*)ACKbuffer)[0] = crc32buf(reinterpret_cast<const char *>(ACKbuffer + 4), 5);
    SPDLOG_DEBUG("Send ACK for {}", seqno);

    _send(ACKbuffer, 9);

//    Add to bytebuffer
}

GoBackNReceiver::GoBackNReceiver(shared_ptr<UnreliableNetworkLayer> unreliable_network_layer)
        : ReliableTransportLayerReceiver(unreliable_network_layer) {
    current_n = 0;
    isThreadAlive = shared_ptr<bool>(new bool());
    *isThreadAlive = true;
    notifier = thread(&GoBackNReceiver::notifierFunc, this);


}

GoBackNReceiver::~GoBackNReceiver() {
    *isThreadAlive = false;
    _fake_notify();
    notifier.join();

}

int GoBackNReceiver::recv(char *msg, int len) {
    int i;
    for( i =0;i<len && bytebuffer.size();i++)
        msg[i] = bytebuffer.front(), bytebuffer.pop();
    return i;
}
