#include <gtest/gtest.h>
#include "UnreliableNetworkLayer/unreliable_network_layer.h"

// Demonstrate some basic assertions.
TEST(UnreliableNetworkLayer, TotallyReliable)
{
    uint8_t *data = new uint8_t[100]();
    for (int i = 0; i < 100; i++)
        data[i]=i;

    UnreliableNetworkLayer totallyReliable(0, 0);
    totallyReliable.send(data, 100, 0);
    this_thread::sleep_for(chrono::milliseconds(100));
    ASSERT_EQ(totallyReliable.recv(data, 100, 1), 100);

    for (int i = 0; i < 100; i++)
        ASSERT_EQ(data[i], i);
    delete [] data;

}

TEST(UnreliableNetworkLayer, LossyStream)
{
    uint8_t *data = new uint8_t[100]();
    UnreliableNetworkLayer totallyReliable(0.5, 0);
    for (int i = 0; i < 100; i++)
        totallyReliable.send(data, 1, 0);
    this_thread::sleep_for(chrono::milliseconds(100));
    int received = 0, x = 0;
    while ((x = totallyReliable.recv(data, 100, 1)))
        received += x;

    ASSERT_NE(received, 100);
    delete [] data;

}

TEST(UnreliableNetworkLayer, CorruptingStream)
{
    uint8_t *data = new uint8_t[100]();
    UnreliableNetworkLayer totallyReliable(0, 0.5);
    for (int i = 0; i < 100; i++)
        totallyReliable.send(data, 1, 0);
    this_thread::sleep_for(chrono::milliseconds(100));
    int received = 0, x = 0;
    bool uneq = false;
    for (int i = 0; i < 100; i++)
    {
        totallyReliable.recv(data, 1, 1);
        uneq = (uneq||data[0]!=i);
    }

    ASSERT_TRUE(uneq);
    delete [] data;
}

TEST(UnreliableNetworkLayer, ReorderingStream)
{
    uint8_t *data = new uint8_t[100]();
    UnreliableNetworkLayer totallyReliable(0, 0, 30);
    for (int i = 0; i < 100; i++)
        data[0] = i, totallyReliable.send(data, 1, 0);
    this_thread::sleep_for(chrono::milliseconds(100));
    int received = 0, x = 0;
    int ood = 0;
    int cmin = -1;
    for (int i = 0; i < 100; i++)
    {

        received += totallyReliable.recv(data, 1, 1);
        // cout << (int)data[0] << "\n";
        if(data[0] != cmin+1)
            ood++;
        cmin = max(cmin, (int)data[0]);
    }
    // printf("ood: %d\n", ood);
    ASSERT_EQ(received, 100);
    ASSERT_TRUE(ood > 0);
    delete [] data;

}

TEST(UnreliableNetworkLayer, NotifyTest) {
    UnreliableNetworkLayer slowNet(0,0,15000);
    uint8_t * data = new uint8_t [2];
    data[0] = 1;
    slowNet.send(data, 1, 0);
    int recv_no_wait = slowNet.recv(data+1, 1, 1);
    ASSERT_EQ(recv_no_wait, 0);
    slowNet.notify(1);
    int recv_wait = slowNet.recv(data+1,1,1);
    ASSERT_EQ(recv_wait, 1);
    ASSERT_EQ(data[0], data[1]);

}