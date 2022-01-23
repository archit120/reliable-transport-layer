//
// Created by archit-dev on 23-01-2022.
//

#include "helpers/circular_buffer.h"

template <typename  T>
CircularBuffer<T>::CircularBuffer(size_t maxSize) : maxSize(maxSize), internalBuffer(new T[maxSize]) {
}

template<typename T>
int CircularBuffer<T>::extend(T * src, size_t len) {
    if(_full)
        return 0;
    if (endPos < startPos) {
        int alen = min(len, startPos-endPos);
        memcpy(internalBuffer.get()+endPos, src, alen);
        endPos += alen;
        if(endPos == startPos)
            _full = true;
        return alen;
    } else {
        auto lenTillEnd = maxSize - endPos;
        int alen = min(lenTillEnd, len);
        memcpy(internalBuffer.get()+endPos, src, alen);
        endPos += alen;
        endPos %= maxSize;
        if(endPos==startPos)
            _full = true;
        if(alen != len) {
            return alen + extend(src+alen, len-alen);
        }
        return alen;
    }
}

template<typename T>
int CircularBuffer<T>::remove(T * dst, size_t size) {
    if(startPos >= endPos) {
        if(startPos == endPos && !_full)
            return 0;
        int alen = min(size, maxSize-startPos);
        memcpy(dst, internalBuffer.get() + startPos, alen);
        startPos += alen;
        startPos %= maxSize;
        _full = false;
        return alen + remove(dst+alen, size-alen);
    }
    if(startPos < endPos) {
        int alen = min(endPos-startPos, size);
        memcpy(dst, internalBuffer.get()+startPos, alen);
        startPos += alen;
        _full = false;
        return alen;
    }
    return 0;
}

template<typename T>
bool CircularBuffer<T>::full() {
    return _full;
}

template<typename T>
size_t CircularBuffer<T>::len() {
    if(_full)   return maxSize;
    return (endPos-startPos+maxSize)%maxSize;
}

template class CircularBuffer<uint8_t>;