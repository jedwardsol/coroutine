#include <iostream>
#include <iomanip>
#include <coroutine>
#include <thread>
#include <cassert>
#include "callstack/callstack.h"


__forceinline void printStack()
{
    auto stack = callstack::get();

    std::reverse(stack.begin(),stack.end());

    if(stack.back() == "printStack")
    {
        stack.pop_back();
    }


    for(const auto &frame : stack)
    {
        std::cout << " " << frame ;
    }

    std::cout << "\n";
}





namespace jle
{

// this is a copy of std::suspend_always   (with `constexpr` removed)

struct suspend_always 
{
    suspend_always()
    {
//      printStack();
    }

    _NODISCARD  bool await_ready() const noexcept 
    {
        printStack();
        return false;
    }

    void await_suspend(std::coroutine_handle<>) const noexcept 
    {
        printStack();
    }
    
    void await_resume() const noexcept 
    {
        printStack();
    }
};


}

struct resumable
{
    struct promise_type                                                     // must be called this
    {
        using coro_handle = std::coroutine_handle<promise_type>;            

        int value{};            

        promise_type()
        {
            printStack();
        }

        auto get_return_object()                                            // must be called this
        {
            printStack();
            return coro_handle::from_promise(*this);
        }

        auto initial_suspend()                                              // must be called this
        { 
            printStack();
            return jle::suspend_always(); 
        }

        auto final_suspend() noexcept                                       // must be called this
        { 
            printStack();
            return jle::suspend_always(); 
        }


        void unhandled_exception()                                          // must be called this
        {
            printStack();
            std::terminate();
        }


        auto yield_value(int i)                                          // must be called this - called on co_yield
        {
            printStack();
            value=i;
            return jle::suspend_always(); 
        }

        void return_value(int i)                                          // must be called this - called on co_return
        {
            printStack();
            value=i;
        }
    };



    using coro_handle = std::coroutine_handle<promise_type>;


    resumable(coro_handle handle) : handle(handle) 
    { 
        printStack();
    }

    ~resumable() 
    { 
        handle.destroy(); 
    }


    resumable(const resumable  &)            = delete;
    resumable(      resumable &&)            = delete;
    resumable &operator=(const resumable  &) = delete;
    resumable &operator=(      resumable &&) = delete;


    bool resume() 
    {
        printStack();

        if (!handle.done())
        {
            handle.resume();
        }

        return !handle.done();
    }


    int value()
    {
        printStack();
        return handle.promise().value;
    }


private:
    coro_handle handle;
};





resumable go()
{
    std::cout << "coroutine starting\n";
    int pi[]={3,1,4,1,5,9};

    for(int index=0;index<5;index++)
    {
        std::cout << "coroutine yielding\n";
        co_yield pi[index];
    }

    std::cout << "coroutine returning\n";
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

}