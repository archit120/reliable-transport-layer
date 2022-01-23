//
// Created by archit-dev on 23-01-2022.
//

#include "helpers/circular_buffer.h"

template <typename  T>
CircularBuffer<T>::CircularBuffer(size_t maxSize) : maxSize(maxSize), internalBuffer(new T[maxSize]) {
}

template<typename T>
int CircularBuffer<T>::extend(T *, int len) {
    return 0;
}

template<typename T>
int CircularBuffer<T>::shift(T *, int size) {
    return 0;
}

template<typename T>
bool CircularBuffer<T>::full() {
    if((endPos+1)%maxSize==startPos)
        return true;
    return false;
}
