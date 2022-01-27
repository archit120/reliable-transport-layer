//
// Created by archit-dev on 23-01-2022.
//

#ifndef RELIABLE_TRANSPORT_LAYER_CIRCULAR_BUFFER_H
#define RELIABLE_TRANSPORT_LAYER_CIRCULAR_BUFFER_H

#include <memory>
#include <cstring>
#include <algorithm>

using namespace std;

template<typename T>
class CircularBuffer {
private:
    unique_ptr<T[]> internalBuffer;
    size_t startPos=0, endPos=0;
    size_t maxSize;
    bool _full = false;
public:
    CircularBuffer(size_t maxSize);

    int extend(T*, size_t len);

    int remove(T*, size_t size);

    bool full();

    size_t len();
};


#endif //RELIABLE_TRANSPORT_LAYER_CIRCULAR_BUFFER_H
