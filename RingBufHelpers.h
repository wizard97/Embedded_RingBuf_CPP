#ifndef EM_RINGBUF_HELPERS_CPP_H
#define EM_RINGBUF_HELPERS_CPP_H


#ifdef ARDUINO
    #include <Arduino.h>
#elif (defined(NRF51) || defined(NRF52) || defined(NRF52_SERIES))
    // One of those defines is mandatory otherwise error is thrown in nrf.h
    #ifndef NORDIC_NRF5x
        #define NORDIC_NRF5x 1 // Common include for Nordic nRF5 SDK
    #endif // NORDIC_NRF5x

    #include "app_util_platform.h"
#else
    #include <stdint.h> // NOLINT
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
        #warning "This library only fully supports AVR and ESP8266 Boards."
        #warning "Operations on the buffer in ISRs are not safe!"
    #endif

#elif defined(NORDIC_NRF5x)
    #define RB_ATOMIC_START CRITICAL_REGION_ENTER()
    #define RB_ATOMIC_END CRITICAL_REGION_EXIT()
#else
    #define RB_ATOMIC_START {
    #define RB_ATOMIC_END }
    #warning "Operations on the buffer in ISRs are not safe!"
    #warning "Implement RB_ATOMIC_START and RB_ATOMIC_END macros for safe ISR operation!"
#endif

#endif
