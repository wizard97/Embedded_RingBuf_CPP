#ifndef RINGBUG_FIFO_H
#define RINGBUF_FIFO_H

#include <Arduino.h>

template<typename Type, uint16_t MaxElements>

class RingBuf
{
public:

RingBuf()
: _buf{ 0 }
{
    _numElements = 0;

    _head = 0;
}


bool add(Type &obj)
{
    if (isFull())
        return false;

    _buf[_head] = obj;
    _head = (_head + 1)%MaxElements;
    numElements++;
    return true;
}

bool pull(Type *dest)
{
    uint16_t tail;

    if (isEmpty())
        return false;

    tail = getTail();

    *dest = _buf[tail];
    numElements--;
    return true;
}

bool isFull()
{
    return numElements >= MaxElements;
}


uint16_t numElements()
{
    return numElements;
}


bool isEmpty()
{
    return !numElements;
}


Type *peek(uint16_t index)
{
    if (index >= numElements)
        return NULL;

    return &_buf[(getTail() + index)%MaxElements];
}

protected:
uint16_t getTail()
{
    return (_head + (MaxElements - numElements))%MaxElements;
}

Type _buf[MaxElements];
uint16_t _head, _tail;

uint16_t _numElements;
private:

};
#endif
