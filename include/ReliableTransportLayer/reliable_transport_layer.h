#ifndef RELIABLE_TRANSPORT_LAYER_H
#define RELIABLE_TRANSPORT_LAYER_H

#include "UnreliableNetworkLayer/unreliable_network_layer.h"
using namespace std;

class ReliableTransportLayer {
    private:
        UnreliableNetworkLayer _unreliable_network_layer;
    public:
        int send(const void *msg, int len); 
        int recv(void *buf, int len);
};

#endif