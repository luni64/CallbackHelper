#pragma once
#include "Arduino.h"
#include "Callback.h"
#include <memory>
// #include <type_traits>

// // trait to check for callable objects c++17 would contain the
// template<typename T, typename = void>
// struct is_callable : std::is_function<T> { };

// template<typename T>
// struct is_callable<T, typename std::enable_if<std::is_same<decltype(void(&T::operator())), void>::value>::type> : std::true_type { };


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

    // generates a callback from anything with an operator()() const. I.e., lambdas, functors ...
    template <typename T>
    callback_t* makeCallback(const T& l, unsigned slot)
    {
        //static_assert(is_callable<T>::value, "class CallbackHelper: The object passed to makeCallback is not callable");
        static_assert(sizeof(T) <= slotSize, "class CallbackHelper: The object passed to makeCallback is too large");

        return new (memoryPool + slot * slotSize) Callback_lambda<T>(l); // please note that placement new does not allocate memory!
    }

 protected:
    uint8_t memoryPool[slotSize * nrOfSlots]; // since we want to avoid dynamic memory allocation we need to reserve the required memory for the delegates statically
};