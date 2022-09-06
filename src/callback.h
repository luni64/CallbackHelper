#pragma once

// abstract base class which will be used to invoke the callback
template <typename T>
class Callback;

template <typename... Args>
struct Callback<void(Args...)>
{
    virtual void invoke(Args...) = 0;
};

// actual callback class which stores the passed in callable (lambda, functor, free functions)
template <typename T, typename... Args>
class CallbackImpl : public Callback<void(Args...)>
{
 public:
    CallbackImpl(T&& _lambda)                
        : lambda(std::forward<T>(_lambda))
    {
    }

    void invoke(Args... args)
    {
        lambda(args...);
    }

 protected:
    T lambda;
};