#pragma once
#include "Arduino.h"
#include "callback.h"
#include <array>
#include <memory>

// /**
//  * To avoid dynamic memory allocation we preallocate a static memory pool and construct the objects into this pool using 'placement new'
//  * Memory usage:
//  *   free function:             ??
//  *   capture less lambda:       1 byte
//  *   capturing lambda:          total size of captured parameters.
//  *   functor:                   total size of class member variables
//  **/

template <typename T, size_t nrOfSlots, size_t slotSize = 16>
class CallbackHelper;

template <typename... Args, size_t nrOfSlots, size_t slotSize>
class CallbackHelper<void(Args...), nrOfSlots, slotSize>
{
 public:
    using signature  = void(Args...);
    using callback_t = Callback<signature>;

    template <typename T>
    callback_t* makeCallback(T&& lbd, size_t slot)
    {
        // make sure user doesn't pass a too large object (e.g. lambda with too much captured parameters) write out of bounds or a slot number larger than nrOfSlots
        if (slot >= nrOfSlots) return nullptr;
        static_assert(sizeof(CallbackImpl<T, Args...>) <= slotSize,
                      "\n****************************************************************************************************"
                      "\n* CallbackHelper::makeCallback Object too large. Consider increasing slot space for CallbackHelper *"
                      "\n**************************************************************************************************** ");

        return new (&(memoryPool[slot])) CallbackImpl<T, Args...>(std::forward<T>(lbd));
    }

 protected:
    //  std::array<uint8_t, slotSize> slotPool;
    //  std::array<std::array<uint8_t, slotSize>,nrOfSlots> memoryPool;

    uint8_t memoryPool[nrOfSlots][slotSize]; // since we want to avoid dynamic memory allocation we need to reserve the required memory for the delegates statically
};
