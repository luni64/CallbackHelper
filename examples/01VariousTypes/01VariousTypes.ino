#include "Arduino.h"
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

// Using a functor as callback----------------------------------------------------
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
    int i = 0;
};
MyFunctor_t functor;

//------------------------------------------------------

// some aliases to save typing
using callbackHelper_t = CallbackHelper<void(void), 5>; // handles 5 slots for void(void) callbacks
using callback_t       = callbackHelper_t::callback_t;  // type of the callbacks

callbackHelper_t cbh; // helps to generate callbacks from various parameters (function pointers, lambdas, functors...)
callback_t* cb[5];    // array to store pointers to the generated callbacks

void setup()
{
    // generate various callbacks and store pointers to them in the cb array
    cb[0] = cbh.makeCallback(freeFunction, 0);                                    // free function -> to callback_t
    cb[1] = cbh.makeCallback(functor, 1);                                         // functor -> callback_t
    cb[2] = cbh.makeCallback([] { Serial.printf("non capturing lambda\n"); }, 2); // simple, non capturing lambda expression -> callback_t
    cb[3] = cbh.makeCallback([] { test.someMemberFunction(3.1415); }, 3);         // non static member function -> callback_t

    int i = 42;
    cb[4] = cbh.makeCallback([i] { Serial.printf("capturing lambda i=%d\n", i); }, 4); // lambda capturing a variable as callback
}

void loop()
{
    for (int i = 0; i < 5; i++)
    {
        cb[i]->invoke(); // invoke stored callback
    }
    Serial.println();

    test.i = millis();     // change state of the test class on the fly
    functor.set(millis()); // change state of the functor on the fly
    delay(500);
}