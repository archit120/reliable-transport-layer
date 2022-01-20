#ifndef RELIABLE_TRANSPORT_LAYER_SENDER_H
#define RELIABLE_TRANSPORT_LAYER_SENDER_H

#include "UnreliableNetworkLayer/unreliable_network_layer.h"
using namespace std;

class ReliableTransportLayerSender
{
private:
    shared_ptr<UnreliableNetworkLayer> _unreliable_network_layer;
protected:

    int _recv(void *buf, int len)
    {
        return _unreliable_network_layer->recv(buf, len, 0);
    }

    int _send(const void *msg, int len)
    {
        return _unreliable_network_layer->send(msg, len, 0);
    }

    int _notify() {
        return _unreliable_network_layer->notify(0);
    }
    int _fake_notify() {
        return _unreliable_network_layer->fake_notify(0);
    }

public:
    ReliableTransportLayerSender(shared_ptr<UnreliableNetworkLayer> unreliable_network_layer) : _unreliable_network_layer(unreliable_network_layer)
    {
    }

    int send(const void *msg, int len);
};

#endif