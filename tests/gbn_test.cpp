//
// Created by archit-dev on 18-01-2022.
//

#include <gtest/gtest.h>
#include "ReliableTransportLayerImplementations/go_back_n.h"

string test_string = "TEST";
char received[10000];

TEST(GoBackN, TotallyReliableTransmit) {
    auto unreliableNetworkLayer = make_shared<UnreliableNetworkLayer>(0);
    GoBackNSender goBackNSender(10, unreliableNetworkLayer);
    GoBackNReceiver goBackNReceiver(unreliableNetworkLayer);

    goBackNSender.send(test_string.c_str(), test_string.size());
    char received[100];
    this_thread::sleep_for(chrono::milliseconds (500));
    int len = goBackNReceiver.recv(received, 100);
    received[len] = 0;
    ASSERT_EQ(len, test_string.size());
    ASSERT_EQ(string(received), test_string);
}

TEST(GoBackN, CorruptingTransmit) {
    auto unreliableNetworkLayer = make_shared<UnreliableNetworkLayer>(0, 0.1);
    GoBackNSender goBackNSender(10, unreliableNetworkLayer);
    GoBackNReceiver goBackNReceiver(unreliableNetworkLayer);

    goBackNSender.send(test_string.c_str(), test_string.size());
    char received[100];
    this_thread::sleep_for(chrono::milliseconds (500));
    int len = goBackNReceiver.recv(received, 100);
    received[len] = 0;
    ASSERT_EQ(len, test_string.size());
    ASSERT_EQ(string(received), test_string);
}

TEST(GoBackN, LongTransmit) {
    string longTest = "";
    for(int i = 0;i<6500; i++) {
        longTest += 'a' + i%26;
    }
    auto unreliableNetworkLayer = make_shared<UnreliableNetworkLayer>();
    GoBackNSender goBackNSender(10, unreliableNetworkLayer);
    GoBackNReceiver goBackNReceiver(unreliableNetworkLayer);

    goBackNSender.send(longTest.c_str(), longTest.size());
    this_thread::sleep_for(chrono::milliseconds (500));
    int len = goBackNReceiver.recv(received, 8000);
    received[len] = 0;
    ASSERT_EQ(len, longTest.size());
}

TEST(GoBackN, LongTransmitCorruptingLossy) {
    string longTest = "";
    for(int i = 0;i<3500; i++) {
        longTest += 'a' + i%26;
    }
    auto unreliableNetworkLayer = make_shared<UnreliableNetworkLayer>(0.2, 0.0005);
    GoBackNSender goBackNSender(10, unreliableNetworkLayer);
    GoBackNReceiver goBackNReceiver(unreliableNetworkLayer);

    goBackNSender.send(longTest.c_str(), longTest.size());
    this_thread::sleep_for(chrono::milliseconds (500));
    int len = goBackNReceiver.recv(received, 4000);
    received[len] = 0;
    ASSERT_EQ(len, longTest.size());
    ASSERT_EQ(string(received), longTest);
}

TEST(GoBackN, LongTransmitCorruptingLossyReordering) {
    string longTest = "";
    for(int i = 0;i<8500; i++) {
        longTest += 'a' + i%26;
    }
    auto unreliableNetworkLayer = make_shared<UnreliableNetworkLayer>(0.2, 0.0005, 30);
    GoBackNSender goBackNSender(10, unreliableNetworkLayer);
    GoBackNReceiver goBackNReceiver(unreliableNetworkLayer);

    goBackNSender.send(longTest.c_str(), longTest.size());
//    this_thread::sleep_for(chrono::milliseconds (10));
    int len = goBackNReceiver.recv_block(received, 8500);
    received[len] = 0;
    ASSERT_EQ(len, longTest.size());
    ASSERT_EQ(string(received), longTest);
}
