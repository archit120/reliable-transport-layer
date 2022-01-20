#ifndef RELIABLE_TRANSPORT_LAYER_RECEIVER_H
#define RELIABLE_TRANSPORT_LAYER_RECEIVER_H

#include "UnreliableNetworkLayer/unreliable_network_layer.h"
using namespace std;

class ReliableTransportLayerReceiver
{
private:
    shared_ptr<UnreliableNetworkLayer> _unreliable_network_layer;


protected:
    int _notify() {
        return _unreliable_network_layer->notify(1);
    }

    int _recv(void *buf, int len)
    {
        return _unreliable_network_layer->recv(buf, len, 1);
    }

    int _send(const void *msg, int len)
    {
        return _unreliable_network_layer->send(msg, len, 1);
    }

    int _fake_notify() {
        return _unreliable_network_layer->fake_notify(1);
    }

public:
    ReliableTransportLayerReceiver(shared_ptr<UnreliableNetworkLayer> unreliable_network_layer) : _unreliable_network_layer(unreliable_network_layer)
    {
    }

    int recv(void *buf, int len);
};

#endif