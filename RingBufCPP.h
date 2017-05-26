/*
The MIT License (MIT)

Copyright (c) 2015 D. Aaron Wisner

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

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
*  Append element obj to the buffer
* Return: true on success
*/
bool add(const Type &obj)
{
    bool ret = false;
    RB_ATOMIC_START
    {
        if (!isFull()) {
            _buf[_head] = obj;
            _head = (_head + 1)%MaxElements;
            _numElements++;

            ret = true;
        }
    }
    RB_ATOMIC_END

    return ret;
}

/**
* Convenience alias of add.
*/
bool push(const Type &obj) { return add(obj); }

/**
* Convenience alias of add.
*/
bool append(const Type &obj) { return add(obj); }

/**
*  Prepend element obj to the buffer
* Return: true on success
*/
bool prepend(const Type &obj)
{
    bool ret = false;
    size_t tail;
    RB_ATOMIC_START
    {
        if (!isFull()) {
            _numElements++;
            tail = getTail();
            _buf[tail] = obj;

            ret = true;
        }
    }
    RB_ATOMIC_END

    return ret;
}

/**
* Remove first element from buffer, and copy it to dest. This is the
* inverse of prepend.
* Return: true on success
*/
bool pull(Type *dest = nullptr)
{
    bool ret = false;
    size_t tail;

    RB_ATOMIC_START
    {
        if (!isEmpty()) {
            tail = getTail();
            if (dest)
                *dest = _buf[tail];
            _numElements--;

            ret = true;
        }
    }
    RB_ATOMIC_END

    return ret;
}

/**
* Remove last element from buffer, and copy it to dest. This is the
* inverse of push/add/append.
* Return: true on success
*/
bool pop(Type *dest = nullptr)
{
    bool ret = false;

    RB_ATOMIC_START
    {
        if (!isEmpty()) {
            if (dest)
                *dest = _buf[_head];
            _head = (_head + MaxElements - 1)%MaxElements;
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
