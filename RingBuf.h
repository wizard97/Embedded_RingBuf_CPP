#ifndef RINGBUG_FIFO_H
#define RINGBUF_FIFO_H

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
        #define RB_ATOMIC_END xt_wsr_ps(_savedIS) ;} while(0);

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
