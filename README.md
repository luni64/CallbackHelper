# CallbackHelper

### Purpose:
CallbackHelper is a tool to generate invocable callbacks from various inputs. E.g., pointers to free functions, pointers to static member functions, lambdas (capturing as well as non-capturing) and functors.
CallbackHelper allows library writers to implement std::function like callback APIs without the memory overhead of std::functions.

### Usage:

Simple example:
```c++
#include "CallbackHelper.h"

void freeFunction()
{
    Serial.println("Free function callback");
}

CallbackHelper<5> cbh; // construct a callback helper to store up to 5 callbacks
callback_t* callback;

void setup()
{
    // generate a callback from freeFunction, store it in slot 0 (out of 5) and
    // store a pointer to it in callback:
     callback = cbh.makeCallback(freeFunction, 0);
}

void loop()
{
    callback->invoke();
}

```
Of course, the same would be possible by storing a pointer to the free function and invoke it in `loop()`. The `CallbackHelper` class becomes more useful if we want to support more complex objects as callbacks. E.g., non static member functions, lambdas or functors:

Here an example how to generate a callback from a lambda expression:
```c++
callback = cbh.makeCallback([] { Serial.printf("non capturing lambda\n"); }, 0);
```

Here how to generate a callback from a non static member function (class `Test`, instance `test` member function `void Test::myFunc()` )
```c++
callback = cbh.makeCallback([] {test.myFunc()); }, 0);
```

The callbackHelper class will construct the actual callback in a preallocated memory pool to avoid dynamic memory allocation.  `makeCallback(...)` returns a base class pointer to the generated callback. Thus, user code doesn't need to know the actual type of the generated callback. Instead, it can store the returned base class pointer for later use in a variable of type pointer to `callback_t`.

Here a complete example showing some possibilities:
```c++
#include "CallbackHelper.h"

// Free function callback ------------------------------------------------------
void freeFunction()
{
    Serial.println("Free function callback");
}

// Using a non static member function as callback -------------------------------
class Test
{
 public:
    Test(int _i){
        i = _i;
    }

    void someMemberFunction(float f){
        Serial.printf("non static member function i=%f\n", i * f);
    }

    int i;
};
Test test(42);

//------------------------------------------------------
class MyFunctor_t
{
 public:
    void set(int _i){
        i = _i;
    }

    void operator()() const{
        Serial.printf("Functor i=%d\n", i);
    }

 protected:
    int i = 2;
};
MyFunctor_t functor;

//------------------------------------------------------

CallbackHelper<5> cbh; // helps to generate callbacks from various parameters (function pointers, lambdas, functors...)
callback_t* cb[5];     // array to store pointers to the generated callbacks

void setup()
{
    // generate various callbacks and store pointers to them in the cb array
    cb[0] = cbh.makeCallback(freeFunction, 0);                                    // convert free function to callback_t
    cb[1] = cbh.makeCallback(functor, 1);                                         // convert a functor to callback_t
    cb[2] = cbh.makeCallback([] { Serial.printf("non capturing lambda\n"); }, 2); // simple, non capturing lambda expression to callback_t
    cb[3] = cbh.makeCallback([] { test.someMemberFunction(3.1415); }, 3);         // use non static member function as callback

    int i = 42;
    cb[4] = cbh.makeCallback([i] { Serial.printf("capturing lambda i=%d\n", i); }, 4); // lambda capturing a variable as callback

    functor.set(123); // set some state in the functor
}

void loop()
{
    for (int i = 0; i < 5; i++)
    {
        cb[i]->invoke(); // invoke stored callback
    }
    Serial.println();

    test.i = millis(); // change state of the test class on the fly
    delay(250);
}
```