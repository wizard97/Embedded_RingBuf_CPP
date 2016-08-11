#include "RingBuf.h"

template <typename Type, uint16_t MaxElements>
RingBuf<Type, MaxElements>::RingBuf()
: _buf{ 0 }
{
    _numElements = 0;

    _head = 0;
}

template <typename Type, uint16_t MaxElements>
bool RingBuf<Type, MaxElements>::add(Type &obj)
{
    if (isFull())
        return false;

    _buf[_head] = obj;
    _head = (_head + 1)%MaxElements;
    numElements++;
    return true;
}

template <typename Type, uint16_t MaxElements>
bool RingBuf<Type, MaxElements>::pull(Type *dest)
{
    uint16_t tail;

    if (isEmpty())
        return false;

    tail = getTail();

    *dest = _buf[tail];
    numElements--;
    return true;
}

template <typename Type, uint16_t MaxElements>
bool RingBuf<Type, MaxElements>::isFull()
{
    return numElements >= MaxElements;
}

template <typename Type, uint16_t MaxElements>
uint16_t RingBuf<Type, MaxElements>::numElements()
{
    return numElements;
}

template <typename Type, uint16_t MaxElements>
bool RingBuf<Type, MaxElements>::isEmpty()
{
    return !numElements;
}

template <typename Type, uint16_t MaxElements>
Type *RingBuf<Type, MaxElements>::peek(uint16_t num)
{
    if (num >= numElements)
        return NULL;

    return &_buf[(getTail() + num)%MaxElements];
}

template <typename Type, uint16_t MaxElements>
uint16_t RingBuf<Type, MaxElements>::getTail()
{
    return (_head + (MaxElements - _numElements))%MaxElements;
}
