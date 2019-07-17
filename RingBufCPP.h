#ifndef EM_RINGBUF_CPP_H
#define EM_RINGBUF_CPP_H

#include "RingBufHelpers.h"

template <typename Type, size_t MaxElements>
class RingBufCPP
{
public:

RingBufCPP()
{
     RB_ATOMIC_START
     {
         _numElements = 0;

         _head = 0;
     }
     RB_ATOMIC_END
}

/**
* Add element obj to the buffer.
*
* If there is already MaxElements in the buffer,
* the oldest element will either be overwritten (when overwrite is true) or
* this add will have no effect (when overwrite is false).
*
* Return: true if there was room in the buffer to add this element
*/
bool add(const Type &obj, bool overwrite=false)
{
    bool full = false;
    RB_ATOMIC_START
    {
        full = isFull();
        if (!full || overwrite) {
            _buf[_head] = obj;
            _head = (_head + 1)%MaxElements;
            _numElements = full ? _numElements : (_numElements + 1);
        }
    }
    RB_ATOMIC_END

    return !full;
}


/**
* Remove last element from buffer, and copy it to dest
* Return: true on success
*/
bool pull(Type *dest)
{
    bool ret = false;
    size_t tail;

    RB_ATOMIC_START
    {
        if (!isEmpty()) {
            tail = getTail();
            *dest = _buf[tail];
            _numElements--;

            ret = true;
        }
    }
    RB_ATOMIC_END

    return ret;
}


/**
* Peek at num'th element in the buffer
* Return: a pointer to the num'th element
*/
Type* peek(size_t num)
{
    Type *ret = NULL;

    RB_ATOMIC_START
    {
        if (num < _numElements) //make sure not out of bounds
            ret = &_buf[(getTail() + num)%MaxElements];
    }
    RB_ATOMIC_END

    return ret;
}


/**
* Return: true if buffer is full
*/
bool isFull() const
{
    bool ret;

    RB_ATOMIC_START
    {
        ret = _numElements >= MaxElements;
    }
    RB_ATOMIC_END

    return ret;
}


/**
* Return: number of elements in buffer
*/
size_t numElements() const
{
    size_t ret;

    RB_ATOMIC_START
    {
        ret = _numElements;
    }
    RB_ATOMIC_END

    return ret;
}


/**
* Return: true if buffer is empty
*/
bool isEmpty() const
{
    bool ret;

    RB_ATOMIC_START
    {
        ret = !_numElements;
    }
    RB_ATOMIC_END

    return ret;
}

protected:
/**
* Calculates the index in the array of the oldest element
* Return: index in array of element
*/
size_t getTail() const
{
    return (_head + (MaxElements - _numElements))%MaxElements;
}


// underlying array
Type _buf[MaxElements];

size_t _head;
size_t _numElements;
private:

};

#endif
