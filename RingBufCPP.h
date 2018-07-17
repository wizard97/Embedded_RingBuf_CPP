#ifndef EM_RINGBUF_CPP_H
#define EM_RINGBUF_CPP_H

#include "RingBufHelpers.h"

/**
 * A simple ring (FIFO) buffer with concurrency protection built in, thus being
 * safe to perform operations on the buffer inside of ISR's. All memory is
 * statically allocated at compile time, so no heap memory is used. It can
 * buffer any fixed size object (ints, floats, structs, objects, etc...).
 *
 * @tparam Type        Type of the elements being stored.
 * @tparam MaxElements Maximum number of elements in this buffer. Note that
 *                     the allocated memory size will be at least
 *                     `MaxElements * sizeof(Type)`.
 */
template<typename Type, size_t MaxElements>
class RingBufCPP {
public:

    RingBufCPP() {
        RB_ATOMIC_START
            {
                _numElements = 0;
                _head = 0;
            }
        RB_ATOMIC_END
    }

    /**
     *  Add an element to the buffer.
     *
     *  @param obj[in] The element to add.
     *
     *  @return true on success.
     */
    bool add(const Type &obj) {
        bool ret = false;

        RB_ATOMIC_START
            {
                if (!isFull()) {
                    _buf[_head] = obj;
                    _head = (_head + 1) % MaxElements;
                    _numElements++;

                    ret = true;
                }
            }
        RB_ATOMIC_END

        return ret;
    }


    /**
     * Remove last element from buffer, and copy it to destination.
     *
     * @param dest[out] Pointer on the allocated object to which removed element
     *                  will be copied.
     *
     * @return true on success.
     */
    bool pull(Type *dest) {
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
     * Peek at n'th element in the buffer.
     *
     * @param num Index of the element to peek at. As this is FIFO buffer, the
     *            oldest element in the buffer is always at index 0 and the
     *            last added one is at the index `numElements() - 1`.
     *
     * @return A pointer to the num'th element or `nullptr` if there is less
     *         elements currently in the buffer than provided index.
     */
    Type *peek(size_t num) {
        Type *ret = nullptr;

        RB_ATOMIC_START
            {
                if (num < _numElements) //make sure not out of bounds
                    ret = &_buf[(getTail() + num) % MaxElements];
            }
        RB_ATOMIC_END

        return ret;
    }


    /**
     * @return true if buffer is full.
     */
    bool isFull() const {
        bool ret;

        RB_ATOMIC_START
            {
                ret = _numElements >= MaxElements;
            }
        RB_ATOMIC_END

        return ret;
    }


    /**
     * @return number of elements currently in buffer.
     */
    size_t numElements() const {
        size_t ret;

        RB_ATOMIC_START
            {
                ret = _numElements;
            }
        RB_ATOMIC_END

        return ret;
    }


    /**
     * @return true if buffer is empty.
     */
    bool isEmpty() const {
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
     * Calculates the index of the oldest element in the array.
     *
     * @return index of the element in array.
     */
    size_t getTail() const {
        return (_head + (MaxElements - _numElements)) % MaxElements;
    }


    /** Underlying array. */
    Type _buf[MaxElements];

    size_t _head;
    size_t _numElements;
private:

};

#endif
