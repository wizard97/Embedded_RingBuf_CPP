#ifndef RINGBUG_FIFO_H
#define RINGBUF_FIFO_H

#include <Arduino.h>

template <typename Type, uint16_t MaxElements>

class RingBuf
{
public:

RingBuf();

bool add(Type &obj);

bool pull(Type *dest);

bool isFull();

uint16_t numElements();

bool isEmpty();

Type *peek(uint16_t index);

protected:
uint16_t getTail();

Type _buf[MaxElements];
uint16_t _head;

uint16_t _numElements;
private:

};
#endif
