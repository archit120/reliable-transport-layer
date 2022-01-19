//
// Created by archit-dev on 18-01-2022.
//

#ifndef RELIABLE_TRANSPORT_LAYER_GENERAL_H
#define RELIABLE_TRANSPORT_LAYER_GENERAL_H
#include <chrono>
#include <iostream>
using namespace  std;
template <
        class result_t   = std::chrono::milliseconds,
        class clock_t    = std::chrono::steady_clock,
        class duration_t = std::chrono::milliseconds
>
result_t since(std::chrono::time_point<clock_t, duration_t> const& start)
{
return std::chrono::duration_cast<result_t>(clock_t::now() - start);
}

inline void dumpPacket(const uint8_t* msg, int len) {
    for(int i =0;i<len;i++)
        cout << (int)msg[i] << " ";
}

#endif //RELIABLE_TRANSPORT_LAYER_GENERAL_H
