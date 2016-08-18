//#include "RingBuf.h"

template <typename Type, uint16_t MaxElements>
RingBuf<Type, MaxElements>::RingBuf()
//: _buf{ 0 }
{
     RB_ATOMIC_START
     {
         _numElements = 0;

         _head = 0;
     }
     RB_ATOMIC_END
}

template <typename Type, uint16_t MaxElements>
bool RingBuf<Type, MaxElements>::add(Type &obj)
{
    RB_ATOMIC_START
    {
        if (isFull())
            return false;

        _buf[_head] = obj;
        _head = (_head + 1)%MaxElements;
        _numElements++;
        return true;
    }
    RB_ATOMIC_END
}

template <typename Type, uint16_t MaxElements>
bool RingBuf<Type, MaxElements>::pull(Type *dest)
{
    RB_ATOMIC_START
    {
        uint16_t tail;

        if (isEmpty())
            return false;

        tail = getTail();

        *dest = _buf[tail];
        _numElements--;
        return true;
    }
    RB_ATOMIC_END
}

template <typename Type, uint16_t MaxElements>
bool RingBuf<Type, MaxElements>::isFull()
{
    RB_ATOMIC_START
    {
        return _numElements >= MaxElements;
    }
    RB_ATOMIC_END
}

template <typename Type, uint16_t MaxElements>
uint16_t RingBuf<Type, MaxElements>::numElements()
{
    RB_ATOMIC_START
    {
        return _numElements;
    }
    RB_ATOMIC_END
}

template <typename Type, uint16_t MaxElements>
bool RingBuf<Type, MaxElements>::isEmpty()
{
    RB_ATOMIC_START
    {
        return !_numElements;
    }
    RB_ATOMIC_END
}

template <typename Type, uint16_t MaxElements>
Type *RingBuf<Type, MaxElements>::peek(uint16_t num)
{
    RB_ATOMIC_START
    {
        if (num >= _numElements)
            return NULL;

        return &_buf[(getTail() + num)%MaxElements];
    }
    RB_ATOMIC_END 
}

// ONLY CALL WHEN SAFE
template <typename Type, uint16_t MaxElements>
uint16_t RingBuf<Type, MaxElements>::getTail()
{
    return (_head + (MaxElements - _numElements))%MaxElements;
}
/*
void TemporaryFunction()
{
    RingBuf<bool, 1> TempObj;
    bool b = true;
    TempObj.add(b);
}
*/
