#pragma once
#include "Arduino.h"
#include "callback.h"
#include <memory>

// /**
//  * To avoid dynamic memory allocation we preallocate a static memory pool and construct the objects into this pool using 'placement new'
//  * Memory usage:
//  *   free function:             ??
//  *   capture less lambda:       1 byte
//  *   capturing lambda:          total size of captured parameters.
//  *   functor:                   total size of class member variables
//  **/

template <  typename T, size_t nrOfSlots, size_t slotSize=16>
class CallbackHelper;

template <typename... Args, size_t nrOfSlots, size_t slotSize>
class CallbackHelper< void(Args...), nrOfSlots, slotSize>
{
 public:
    using signature  = void(Args...);
    using callback_t = Callback<signature>;

    template <typename T>
    callback_t* makeCallback(T&& lbd, size_t slot)
    {
        return new(memoryPool + slot*slotSize) CallbackImpl<T, Args...>(std::forward<T>(lbd));
    }

 protected:
   uint8_t memoryPool[slotSize * nrOfSlots]; // since we want to avoid dynamic memory allocation we need to reserve the required memory for the delegates statically
};
