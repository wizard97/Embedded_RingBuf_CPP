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

#ifndef EM_RINGBUF_HELPERS_CPP_H
#define EM_RINGBUF_HELPERS_CPP_H

// TODO fix this
#ifndef NULL
    #define NULL (void *)(0)
#endif

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

#endif
