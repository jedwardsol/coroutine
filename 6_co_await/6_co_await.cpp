#include <iostream>
#include <iomanip>
//#include <coroutine>
#include <thread>
#include <cassert>
#include "callstack/callstack.h"


#include <coroutine>        // std
#include "coroutine.h"      // jle versions of std types for instrumentation


struct uncopyable
{
    uncopyable()                               = default;
    uncopyable(const uncopyable  &)            = delete;
    uncopyable(      uncopyable &&)            = delete;
    uncopyable &operator=(const uncopyable  &) = delete;
    uncopyable &operator=(      uncopyable &&) = delete;
};


struct justGo  : uncopyable
{
    struct promise_type                                                     // must be called this
    {
        using coro_handle = std::coroutine_handle<promise_type>;            

        auto get_return_object()                                            // must be called this
        {
            return coro_handle::from_promise(*this);
        }

        auto initial_suspend()                                              // must be called this
        { 
            return std::suspend_never();                                    // go ...
        }

        auto final_suspend() noexcept                                       // must be called this
        { 
            return std::suspend_never();                                    // go ... ,  coro_handle will be automatically destroyed so no explicit call in destructor
        }

        void unhandled_exception()                                          // must be called this
        {
            std::terminate();
        }

        void return_void()
        {
        }
    };

    using coro_handle = std::coroutine_handle<promise_type>;

    justGo(coro_handle handle) : handle(handle) 
    {}

private:
    coro_handle handle;
};





struct resumable : uncopyable
{
    struct promise_type                                                     // must be called this
    {
        using coro_handle = jle::coroutine_handle<promise_type>;            

        int value{};            

        promise_type()
        {
            callstack::printStack();
        }

        ~promise_type()
        {
            callstack::printStack();
        }

        auto get_return_object()                                            // must be called this
        {
            callstack::printStack();
            return coro_handle::from_promise(*this);
        }

        auto initial_suspend()                                              // must be called this
        { 
            callstack::printStack();
            return jle::suspend_always(); 
        }

        auto final_suspend() noexcept                                       // must be called this
        { 
            callstack::printStack();
            return jle::suspend_always(); 
        }


        void unhandled_exception()                                          // must be called this
        {
            callstack::printStack();
            std::terminate();
        }


        auto yield_value(int i)                                             // must be called this - called on co_yield
        {
            callstack::printStack();
            value=i;
            return jle::suspend_always(); 
        }

        void return_value(int i)                                            // must be called this - called on co_return
        {
            callstack::printStack();
            value=i;
        }
    };



    using coro_handle = jle::coroutine_handle<promise_type>;


    resumable(coro_handle handle) : handle(handle) 
    { 
        callstack::printStack();
    }

    ~resumable() 
    { 
        callstack::printStack();
        handle.destroy(); 
    }


    operator bool()
    {
        return !handle.done();
    }

    bool await_ready() 
    {
        callstack::printStack();
        return handle.done();
    }


    auto await_suspend(std::coroutine_handle<> caller)                      // caller has called co_await.  This will resume the child.  Then return the parent, so it can be resumed.   (explicit resume results in recursion)
    {
        callstack::printStack();
        handle.resume();

        return caller;
    }

    auto await_resume() 
    {
        callstack::printStack();
        return handle.promise().value;
    }

private:
    coro_handle handle;
};





resumable produce()
{
    std::cout << "produce starting\n";
    int pi[]={3,1,4,1,5,9};

    for(int index=0;index<5;index++)
    {
        std::cout << "produce yielding " << pi[index] << "\n";
        co_yield pi[index];
    }

    std::cout << "produce yielding " << pi[5] << "\n";
    co_return pi[5];
}


justGo consume()
{
    std::cout << "consume\n";

    auto producer = produce();

    while(producer)
    {
        std::cout << "consumer co_awaiting\n";
        auto value = co_await producer;
        std::cout << "consumed " << value << "\n";
    }
}



int main()
{
    callstack::init();

    consume();
}
 