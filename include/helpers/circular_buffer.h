//
// Created by archit-dev on 23-01-2022.
//

#ifndef RELIABLE_TRANSPORT_LAYER_CIRCULAR_BUFFER_H
#define RELIABLE_TRANSPORT_LAYER_CIRCULAR_BUFFER_H

#include <memory>

using namespace std;

template<typename T>
class CircularBuffer {
private:
    unique_ptr<T[]> internalBuffer;
    size_t startPos=0, endPos=0;
    size_t maxSize;
public:
    CircularBuffer(size_t maxSize);

    int extend(T*, int len);

    int shift(T*, int size);

    bool full();
};


#endif //RELIABLE_TRANSPORT_LAYER_CIRCULAR_BUFFER_H
