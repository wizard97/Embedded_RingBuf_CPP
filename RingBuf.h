#ifndef EM_RINGBUF_FIFO_CPP_H
#define EM_RINGBUF_FIFO_CPP_H

// TODO fix this
#define NULL (void *)(0)

#ifdef ARDUINO
    #include <Arduino.h>
#else
    #include <stdint.h>
#endif

#ifdef ARDUINO

    #if defined(ARDUINO_ARCH_AVR)
        #include <util/atomic.h>
        #define RB_ATOMIC_START ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        #define RB_ATOMIC_END }


    #elif defined(ARDUINO_ARCH_ESP8266)
        #ifndef __STRINGIFY
            #define __STRINGIFY(a) #a
        #endif

        #ifndef xt_rsil
            #define xt_rsil(level) (__extension__({uint32_t state; __asm__ __volatile__("rsil %0," __STRINGIFY(level) : "=a" (state)); state;}))
        #endif

        #ifndef xt_wsr_ps
            #define xt_wsr_ps(state)  __asm__ __volatile__("wsr %0,ps; isync" :: "a" (state) : "memory")
        #endif

        #define RB_ATOMIC_START do { uint32_t _savedIS = xt_rsil(15) ;
        #define RB_ATOMIC_END xt_wsr_ps(_savedIS); } while(0);

    #else
        #define RB_ATOMIC_START {
        #define RB_ATOMIC_END }
        #warning “This library only fully supports AVR and ESP8266 Boards.”
        #warning "Operations on the buffer in ISRs are not safe!"
    #endif

#else
    #define RB_ATOMIC_START {
    #define RB_ATOMIC_END }
    #warning "Operations on the buffer in ISRs are not safe!"
    #warning "Impliment RB_ATOMIC_START and RB_ATOMIC_END macros for safe ISR operation!"
#endif

template <typename Type, uint16_t MaxElements>
class RingBuf
{
public:

RingBuf();

/**
*  Add element obj to the buffer
* Return: true on success
*/
bool add(Type &obj);

/**
* Remove last element from buffer, and copy it to dest
* Return: true on success
*/
bool pull(Type *dest);

/**
* Peek at num'th element in the buffer
* Return: a pointer to the num'th element
*/
Type *peek(uint16_t num);

/**
* Return: true if buffer is full
*/
bool isFull();

/**
* Return: number of elements in buffer
*/
uint16_t numElements();

/**
* Return: true if buffer is empty
*/
bool isEmpty();

protected:
/**
* Calculates the index in the array of the oldest element
* Return: index in array of element
*/
uint16_t getTail();

// underlying array
Type _buf[MaxElements];

uint16_t _head;
uint16_t _numElements;
private:

};

#include "RingBuf.cpp"
#endif
