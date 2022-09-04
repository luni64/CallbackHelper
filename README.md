# CallbackHelper

Helper class to generate invocable callbacks from various inputs. E.g., pointers to free functions, pointers to static member functions, lambdas (capturing an non-capturing) and functors.

**Purpose** Allow for std::function like callback APIs without the memory overhead of std::functions.

**Simple usage:**

```c++
#include "Arduino.h"

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
Of course the same would be possible by storing a simple pointer to the free function and invoke it in loop(). It gets more interesting if we want to use more difficult objects as callbacks. E.g., non static member functions, lambdas:

Generate callback from a lambda expression:
```c++
callback = cbh.makeCallback([] { Serial.printf("non capturing lambda\n"); }, 0);
```

Generate callback from non static member function (class `Test`, instance `test` member function `void Test::myFunc()` )
```c++
callback = cbh.makeCallback([] {test.myFunc()); }, 0);
```

Here a complete example showing the possibilities:

```c++
#include "Arduino.h"
#include "callbackHelper.h"

// Free function callback ------------------------------------------------------
void freeFunction()
{
    Serial.println("Free function callback");
}

// Using a non static member function as callback -------------------------------
class Test
{
 public:
    Test(int _i)
    {
        i = _i;
    }

    void someMemberFunction(float f)
    {
        Serial.printf("non static member function i=%f\n", i * f);
    }

    int i;
};
Test test(42);

//------------------------------------------------------
class MyFunctor_t
{
 public:
    void set(int _i)
    {
        i = _i;
    }

    void operator()() const
    {
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


