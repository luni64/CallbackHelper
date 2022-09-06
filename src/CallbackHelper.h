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
        return new CallbackImpl<T, Args...>(std::forward<T>(lbd));
    }

 protected:
  // uint8_t memoryPool[slotSize * nrOfSlots]; // since we want to avoid dynamic memory allocation we need to reserve the required memory for the delegates statically
};

// #pragma once
// #include "Arduino.h"
// #include "callback.h"
// #include <memory>

// /**
//  * To avoid dynamic memory allocation we need to preallocate static memory and construct the objects into this using 'placement new'
//  * Memory usage:
//  *   function pointer delegate: 4bytes
//  *   capture less lambda:       1 byte
//  *   capturing lambda:          total size of captured parameters.
//  *   functor:                   total size of class member variables
//  **/

// template <unsigned nrOfSlots, size_t slotSize = 16> // use the second, optional parameter to change the slot size if needed
// class CallbackHelper
// {
//  public:
//     // generates a callback from a function pointer
//     callback_t* makeCallback(void (*fptr)(), unsigned slot)
//     {
//         return new (memoryPool + slot * slotSize) Callback_fptr(fptr); // please note that placement new does NOT allocate memory!
//     }

//     // tries to generate a callback from anything with an operator()() const. I.e., lambdas, functors ...
//     template<typename T>
//     callback_t* makeCallback(T&& callable, unsigned slot)
//     {
//         static_assert(sizeof(T) <= slotSize, "class CallbackHelper: The object passed to makeCallback is too large");
//         return new (memoryPool + slot * slotSize) callbackImpl_t<T>(std::forward<T>(callable)); // please note that placement new does NOT allocate memory!
//     }

//  protected:
//     uint8_t memoryPool[slotSize * nrOfSlots]; // since we want to avoid dynamic memory allocation we need to reserve the required memory for the delegates statically
// };
