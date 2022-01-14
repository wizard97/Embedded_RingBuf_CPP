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
    bool ret = false;
    
    RB_ATOMIC_START
    {
        full = isFull();
        if (!full || overwrite) {
            _buf[_head] = obj;
            _head = (_head + 1)%MaxElements;
            _numElements = full ? _numElements : (_numElements + 1);
            ret = true;
        }
    }
    RB_ATOMIC_END

    return ret;
}

/**
* Adds "length" elements from the "obj" array to the buffer.
*
* If there is already MaxElements in the buffer,
* the oldest element will either be overwritten (when overwrite is true) or
* this add will have no effect (when overwrite is false).
*
* Return: number of elements actually added to the buffer (could be less than length if overwrite was set to false and MaxElements-_numElements < length)
*/
size_t add(const Type *obj, size_t length, bool overwrite=false)
{
    size_t ret = 0;

    RB_ATOMIC_START
    {
        if(length <= MaxElements) { // Can't write more elements than the buffer size or zero elements.
            if(!overwrite) {
                // If overwrite is disabled, length should be trunked to the available space for writing in the buffer.
                length = (MaxElements-_numElements) > length ? length : (MaxElements-_numElements);
            }

            if(length > 0) {
                if(length > (MaxElements-_head)) {
                    // Needs two memcpy.
                    memcpy(&_buf[_head],obj,sizeof(Type)*(MaxElements-_head)); // From _head to the end of _buf
                    memcpy(_buf,&obj[MaxElements-_head],sizeof(Type)*(length-(MaxElements-_head))); // From the start of _buf until length-(MaxElements-_head)
                }
                else {
                    // Only one copy is enough
                    memcpy(&_buf[_head],obj,sizeof(Type)*length); // From _head to _head+length
                }

                _head = (_head+length) % MaxElements;
                _numElements = (_numElements+length > MaxElements) ? MaxElements : _numElements+length;
                ret = length;
            }
        }
    }
    RB_ATOMIC_END

    return ret;
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
* Remove "length" last elements from buffer, and copy them to "dest" buffer
* Return: number of elements actually copied (could be less if numElements < length).
*/
size_t pull(Type *dest, size_t length)
{
    size_t tail;
    size_t ret = 0;

    RB_ATOMIC_START
    {
        if ((!isEmpty()) && (length > 0)) {
            length = _numElements > length ? length : _numElements; // Trunk length if exceeds elements available for reading.

            tail = getTail();
            if(length > (MaxElements-tail)) {
                // Needs two memcpy.
                memcpy(dest,&_buf[tail],sizeof(Type)*(MaxElements-tail)); // From tail to the end of _buf
                memcpy(&dest[MaxElements-tail],_buf,sizeof(Type)*(length-(MaxElements-tail))); // From the start of _buf until length-(MaxElements-tail)
            }
            else {
                // Only one copy is enough
                memcpy(dest,&_buf[tail],sizeof(Type)*length); // From tail to tail+length
            }

            _numElements = _numElements-length;
            ret = length;
        }
    }
    RB_ATOMIC_END

    return ret;
}

/**
* Peek at num'th element in the buffer
* Return: a pointer to the num'th element (nullptr if num is larger than the number of elements in the buffer)
*/
Type* peek(size_t num)
{
    Type *ret = nullptr;

    RB_ATOMIC_START
    {
        if (num < _numElements) //make sure not out of bounds
            ret = &_buf[(getTail() + num)%MaxElements];
    }
    RB_ATOMIC_END

    return ret;
}

/**
* Peek at oldest element in the buffer.
* Return: a pointer to the element (nullptr if buffer is empty)
*/
Type* peek() {
    Type *ret = nullptr;

    RB_ATOMIC_START
    {
    if (_numElements > 0)
        ret = &_buf[getTail()];
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
        ret = _numElements == MaxElements;
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
