#ifndef EM_RINGBUF_CPP_H
#define EM_RINGBUF_CPP_H

#include "SimplyAtomic/SimplyAtomic.h"

template <typename Type, size_t MaxElements>
class RingBufCPP
{
public:

RingBufCPP()
{
     ATOMIC()
     {
         _numElements = 0;

         _head = 0;
     }
}

/**
*  Append element obj to the buffer
* Return: true on success
*/
bool add(const Type &obj)
{
    bool ret = false;

    ATOMIC()
    {
        if (!isFull()) {
            _buf[_head] = obj;
            _head = (_head + 1)%MaxElements;
            _numElements++;

            ret = true;
        }
    }

    return ret;
}

/**
*  Prepend element obj to the buffer
* Return: true on success
*/
bool prepend(const Type &obj)
{
    bool ret = false;
    size_t tail;
    ATOMIC()
    {
        if (!isFull()) {
            _numElements++;
            tail = getTail();
            _buf[tail] = obj;

            ret = true;
        }
    }

    return ret;
}

/**
* Remove first element from buffer, and copy it to dest. This is the
* inverse of prepend.
* Return: true on success
*/
bool pull(Type *dest)
{
    bool ret = false;
    size_t tail;

    ATOMIC()
    {
        if (!isEmpty()) {
            tail = getTail();
            *dest = _buf[tail];
            _numElements--;

            ret = true;
        }
    }

    return ret;
}


/**
* Remove first element from buffer, and copy it to dest. This is the
* inverse of prepend.
* Return: true on success
*/
bool pull(Type &dest) { return pull(&dest); }


/**
* Remove last element from buffer, and copy it to dest. This is the
* inverse of add.
* Return: true on success
*/
bool pop(Type *dest)
{
    bool ret = false;

    ATOMIC()
    {
        if (!isEmpty()) {
            *dest = _buf[_head];
            _head = (_head + MaxElements - 1)%MaxElements;
            _numElements--;

            ret = true;
        }
    }

    return ret;
}


/**
* Remove last element from buffer, and copy it to dest. This is the
* inverse of add.
* Return: true on success
*/
bool pop(Type &dest) { return pop(&dest); }


/**
* Peek at num'th element in the buffer
* Return: a pointer to the num'th element
*/
Type* peek(size_t num) const
{
    Type *ret = NULL;

    ATOMIC()
    {
        if (num < _numElements) //make sure not out of bounds
            ret = &_buf[(getTail() + num)%MaxElements];
    }

    return ret;
}


/**
* Return: true if buffer is full
*/
bool isFull() const
{
    bool ret;

    ATOMIC()
    {
        ret = _numElements >= MaxElements;
    }

    return ret;
}


/**
* Return: number of elements in buffer
*/
size_t numElements() const
{
    size_t ret;

    ATOMIC()
    {
        ret = _numElements;
    }

    return ret;
}


/**
* Return: true if buffer is empty
*/
bool isEmpty() const
{
    bool ret;

    ATOMIC()
    {
        ret = !_numElements;
    }

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
