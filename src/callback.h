#pragma once

// abstract base class. Pointer to this will be stored externaly to invoke the callback later
struct Callback
{
    virtual void invoke() = 0;
};

// callback type which stores free functions or static member functions
class Callback_fptr : public Callback
{
 public:
    using fp_t = void (*)(); // type of a pointer to a void(void)  function

    Callback_fptr(fp_t fp)
    {
        this->fp = fp;
    }

    void invoke()
    {
        fp();
    }

 protected:
    fp_t fp; // holds the unction pointer passed in
};

// delegate to store capture-less and capturing lambdas
template <typename T>
class Callback_lambda : public Callback
{
 public:
    Callback_lambda(const T& _lambda)
        : lambda(_lambda)
    {
    }

    void invoke()
    {
         lambda();
    }

 protected:
    const T lambda;
};
