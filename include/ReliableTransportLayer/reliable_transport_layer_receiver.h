#ifndef RELIABLE_TRANSPORT_LAYER_RECEIVER_H
#define RELIABLE_TRANSPORT_LAYER_RECEIVER_H

#include "UnreliableNetworkLayer/unreliable_network_layer.h"
using namespace std;

class ReliableTransportLayerReceiver
{
private:
    shared_ptr<UnreliableNetworkLayer> _unreliable_network_layer;

    int _recv(void *buf, int len)
    {
        _unreliable_network_layer->recv(buf, len, 1);
    }

    int _send(const void *msg, int len)
    {
        _unreliable_network_layer->send(msg, len, 1);
    }

public:
    ReliableTransportLayerReceiver(shared_ptr<UnreliableNetworkLayer> unreliable_network_layer) : _unreliable_network_layer(unreliable_network_layer)
    {
    }

    int recv(void *buf, int len);
};

#endif