#include "Arduino.h"
#include "PIT.h"

/*************************************************************************
 * This example shows how to use the CallbackHelper library to provide
 * a std::function like API to a hardware timer (here a T4.x PIT).
 * Users can attach free functions, functors, lambdas etc to the PIT class
 *
 *************************************************************************/

// standard free function which can be attached as callback.
void freeFunction()
{
    Serial.println("Free function called");
}

// Functors are classes with an overloaded function call operator.
// Before lambda expressions where available, functors were often used
// as callbacks which can store state.
class functor_t
{
 public:
    void operator()()
    {
        Serial.printf("Functor called i:%d\n", i);
    }
    int i = 17;
};
functor_t myFunctor;

PIT myPIT;

void setup()
{
    // to test, uncomment one of the examples below

    myPIT.begin(freeFunction, 250'000); // attach a free void(void) function as callback
    // myPIT.begin(myFunctor, 250'000);                                                   // invoke the function call operator of myFunctor every 250ms
    // myPIT.begin([] { Serial.println("captureless lambda called"); }, 250'000);         // attach a simple lambda expression
    // int i = 42;
    // myPIT.begin([i] { Serial.printf("capturing lambda called i:%d\n", i); }, 250'000); // attach a lambda expression, capturing a variable value
}

void loop()
{
    Serial.println("---------------");
    myFunctor.i = millis(); // change some state of the functor on the fly
    delay(2000);
}
