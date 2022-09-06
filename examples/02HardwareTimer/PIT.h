#pragma once
#include "callbackHelper.h"

class PIT
{
 public:
    template <class T>
    void begin(T&& lambda, float period)                         // lambdas, functors etc.
    {                                                            //
        callback = cbh.makeCallback(std::forward<T>(lambda), 0); // make a delegate from the parameters and store for calling from the ISR
        initPIT(period);                                         // setup PIT module
    }

 protected:
    static void isr()           // actual ISR
    {                           //
        if (PIT_TFLG0)          // did we get a timer event for channel 0
        {                       //
            PIT_TFLG0 = 1;      // reset the interrupt flag
            callback->invoke(); // invoke the stored callback
        }
        asm volatile("dsb"); // wait until register changes propagated through the cache to avoid double calls
    }

    void initPIT(float period)
    {
        static bool isInitialized = false;

        if (!isInitialized)
        {
            CCM_CCGR1 |= CCM_CCGR1_PIT(CCM_CCGR_ON);
            PIT_MCR = 1;

            attachInterruptVector(IRQ_PIT, isr);
            NVIC_ENABLE_IRQ(IRQ_PIT);

            isInitialized = true;
        }

        PIT_LDVAL0 = period * 24;                   // reload value, period in microseconds, timer clock 24MHz
        PIT_TCTRL0 = PIT_TCTRL_TEN | PIT_TCTRL_TIE; // enable channel and generate interrupts on overflow
    }

    using callbackHelper_t = CallbackHelper<void(void), 5>; // handles 5 slots for void(void) callbacks
    using callback_t       = callbackHelper_t::callback_t;  // type of the callbacks

    callbackHelper_t cbh;       // use a callback helper which manages the callback for the PIT (for the sake of simplicity we only use PIT channel 0 here)
    static callback_t* callback; // in a real implementation this would be a static array of 4 callback_t pointers
};

PIT::callback_t* PIT::callback = nullptr;
