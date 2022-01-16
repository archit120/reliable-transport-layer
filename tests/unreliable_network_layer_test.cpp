#include <gtest/gtest.h>
#include "UnreliableNetworkLayer/unreliable_network_layer.h"

// Demonstrate some basic assertions.
TEST(UnreliableNetworkLayer, TotallyReliable)
{
    uint8_t *data = new uint8_t[100]();
    for (int i = 0; i < 100; i++)
        data[i]=i;

    UnreliableNetworkLayer totallyReliable(0, 0);
    totallyReliable.send(data, 100);
    this_thread::sleep_for(chrono::milliseconds(100));
    ASSERT_EQ(totallyReliable.recv(data, 100), 100);

    for (int i = 0; i < 100; i++)
        ASSERT_EQ(data[i], i);
}

TEST(UnreliableNetworkLayer, LossyStream)
{
    uint8_t *data = new uint8_t[100]();
    UnreliableNetworkLayer totallyReliable(0.5, 0);
    for (int i = 0; i < 100; i++)
        totallyReliable.send(data, 1);
    this_thread::sleep_for(chrono::milliseconds(100));
    int received = 0, x = 0;
    while ((x = totallyReliable.recv(data, 100)))
        received += x;

    ASSERT_NE(received, 100);
}

TEST(UnreliableNetworkLayer, CorruptingStream)
{
    uint8_t *data = new uint8_t[100]();
    UnreliableNetworkLayer totallyReliable(0, 0.5);
    for (int i = 0; i < 100; i++)
        totallyReliable.send(data, 1);
    this_thread::sleep_for(chrono::milliseconds(100));
    int received = 0, x = 0;
    bool uneq = false;
    for (int i = 0; i < 100; i++)
    {
        totallyReliable.recv(data, 1);
        uneq = (uneq||data[0]!=i);
    }

    ASSERT_TRUE(uneq);
}

TEST(UnreliableNetworkLayer, ReorderingStream)
{
    uint8_t *data = new uint8_t[100]();
    UnreliableNetworkLayer totallyReliable(0, 0, 5);
    for (int i = 0; i < 100; i++)
        totallyReliable.send(data, 1);
    this_thread::sleep_for(chrono::milliseconds(100));
    int received = 0, x = 0;
    bool uneq = false;
    for (int i = 0; i < 100; i++)
    {
        received += totallyReliable.recv(data, 1);
        uneq = (uneq||data[0]!=i);
    }
    ASSERT_EQ(received, 100);
    ASSERT_TRUE(uneq);
}
