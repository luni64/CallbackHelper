#pragma once

// abstract base class. Pointer to this will be stored externally to invoke the callback later
struct callback_t
{
    virtual void invoke() = 0;
};

// callback type which stores free functions or static member functions
class Callback_fptr : public callback_t
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
template <typename lambda_t>
class Callback_lambda : public callback_t
{
 public:
    Callback_lambda(lambda_t&& _lambda)
        : lambda(std::forward<lambda_t>(_lambda))
    {}

    void invoke() { lambda(); }

 protected:
    lambda_t lambda;
};
