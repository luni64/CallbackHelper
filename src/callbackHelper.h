#pragma once
#include "Arduino.h"
#include "Callback.h"
#include <memory>

/**
 * To avoid dynamic memory allocation we need to prealocate static memory and construct the objects into this using 'placement new'
 * Memory usage:
 *   function pointer delegate: 4bytes
 *   capture less lambda:       1 byte
 *   capturing lambda:          total size of captured parameters.
 *   functor:                   total size of class member variables
 **/


template <unsigned nrOfSlots, size_t slotSize = 16> // use the second, optional parameter to change the slotsize if needed
class CallbackHelper
{
 public:
    // generates a callback from a function pointer
    callback_t* makeCallback(void (*fptr)(), unsigned slot)
    {
        return new (memoryPool + slot * slotSize) Callback_fptr(fptr); // please note that placement new does not allocate memory!
    }

    // tries to generate a callback from anything with an operator()() const. I.e., lambdas, functors ...
    template<typename T>
    callback_t* makeCallback(T&& l, unsigned slot)
    {
        static_assert(sizeof(T) <= slotSize, "class CallbackHelper: The object passed to makeCallback is too large");
        return new (memoryPool + slot * slotSize) Callback_lambda<T>(std::forward<T>(l)); // please note that placement new does not allocate memory!
    }


 protected:
    uint8_t memoryPool[slotSize * nrOfSlots]; // since we want to avoid dynamic memory allocation we need to reserve the required memory for the delegates statically
};
