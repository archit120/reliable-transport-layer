//
// Created by archit-dev on 18-01-2022.
//

#ifndef RELIABLE_TRANSPORT_LAYER_GENERAL_H
#define RELIABLE_TRANSPORT_LAYER_GENERAL_H
#include <chrono>
template <
        class result_t   = std::chrono::milliseconds,
        class clock_t    = std::chrono::steady_clock,
        class duration_t = std::chrono::milliseconds
>
result_t since(std::chrono::time_point<clock_t, duration_t> const& start)
{
return std::chrono::duration_cast<result_t>(clock_t::now() - start);
}

#endif //RELIABLE_TRANSPORT_LAYER_GENERAL_H
