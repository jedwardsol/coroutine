#include <iostream>
#include <coroutine>
#include <thread>
#include <cassert>

#include "callstack/callstack.h"




struct resumable
{
    struct promise_type                                                     // must be called this
    {
        using coro_handle = std::coroutine_handle<promise_type>;            

        int value{};            

        promise_type()
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
            return std::suspend_always(); 
        }

        auto final_suspend() noexcept                                       // must be called this
        { 
            callstack::printStack();
            return std::suspend_always(); 
        }


        void unhandled_exception()                                          // must be called this
        {
            callstack::printStack();
            std::terminate();
        }


        auto yield_value(int i)                                          // must be called this - called on co_yield
        {
            callstack::printStack();
            value=i;
            return std::suspend_always(); 
        }

        void return_value(int i)                                          // must be called this - called on co_return
        {
            callstack::printStack();
            value=i;
        }
    };





    using coro_handle = std::coroutine_handle<promise_type>;


    resumable(coro_handle handle) : handle(handle) 
    { 
            callstack::printStack();
    }

    ~resumable() 
    { 
            callstack::printStack();
        handle.destroy(); 
    }


    resumable(const resumable  &)            = delete;
    resumable(      resumable &&)            = delete;
    resumable &operator=(const resumable  &) = delete;
    resumable &operator=(      resumable &&) = delete;


    bool resume() 
    {
        callstack::printStack();

        if (!handle.done())
        {
            handle.resume();
        }

        return !handle.done();
    }


    int value()
    {
        return handle.promise().value;
    }


private:
    coro_handle handle;
};





resumable go()
{
    int pi[]={3,1,4,1,5,9};

    for(int index=0;index<5;index++)
    {
        co_yield pi[index];
    }

    co_return pi[5];
}


int main()
{
    callstack::init();
 
    std::cout << "calling go for 1st time\n";
    resumable res = go();
    std::cout << "called go for 1st time\n";


    while ( std::cout << "main resuming\n" && res.resume())
    {
        auto value = res.value();

        std::cout << "main got " << value << '\n';
    }


    std::cout << "Final value was " << res.value() << std::endl;
  
  }