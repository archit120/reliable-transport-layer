#include <gtest/gtest.h>
#include "UnreliableNetworkLayer/unreliable_network_layer.h"

// Demonstrate some basic assertions.
TEST(UnreliableNetworkLayer, TotallyReliable) {
    uint8_t* data = new uint8_t[100]();
    UnreliableNetworkLayer totallyReliable(0, 0, 100);
    totallyReliable.send(data, 100);
    this_thread::sleep_for(chrono::milliseconds(100));
    totallyReliable.recv(data, 100);

    for(int i =0;i<100;i++)
        ASSERT_EQ(data[i], 0);
}
