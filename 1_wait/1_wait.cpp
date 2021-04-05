#include <iostream>
#include <coroutine>
#include <thread>
#include <cassert>

struct resumable
{
    struct promise_type                                                     // must be called this
    {
        using coro_handle = std::coroutine_handle<promise_type>;            


        promise_type()
        {
            std::cout << __FUNCTION__  << "\n";
        }

        auto get_return_object()                                            // must be called this
        {
            std::cout << __FUNCTION__  << "\n";
            return coro_handle::from_promise(*this);
        }

        auto initial_suspend()                                              // must be called this
        { 
            std::cout << __FUNCTION__ << "\n";
            return std::suspend_always(); 
        }

        auto final_suspend() noexcept                                       // must be called this
        { 
            std::cout << __FUNCTION__ << "\n";
            return std::suspend_always(); 
        }

        void return_void()                                                  // must be called this
        {
            std::cout << __FUNCTION__ << "\n";
        }

        void unhandled_exception()                                          // must be called this
        {
            std::cout << __FUNCTION__ << "\n";
            std::terminate();
        }
    };



    using coro_handle = std::coroutine_handle<promise_type>;


    resumable(coro_handle handle) : handle(handle) 
    { 
        std::cout << __FUNCTION__ << "\n";
    }

    ~resumable() 
    { 
        std::cout << __FUNCTION__ << "\n";
        handle.destroy(); 
    }


    resumable(const resumable  &)            = delete;
    resumable(      resumable &&)            = delete;
    resumable &operator=(const resumable  &) = delete;
    resumable &operator=(      resumable &&) = delete;


    bool resume() 
    {
        std::cout << __FUNCTION__ << "\n";

        if (!handle.done())
        {
            handle.resume();
                // 12 : calls std::coroutine_handle<resumable::promise_type>::resume
                // 13 :     calls go()

                // 19 : calls std::coroutine_handle<resumable::promise_type>::resume
                // 20 :     calls go()

                // 26 : calls std::coroutine_handle<resumable::promise_type>::resume
                // 27 :     calls go()
        }

        return !handle.done();
    }



private:
    coro_handle handle;
};





resumable go()
{
    //  2 : allocates state
    //  3 : calls go$_InitCoro$2 
    //  4 :     creates a resumable::promise_type
    //  5 :     calls resumable::promise_type::get_return_object 
    //  6 :     calls resumable::promise_type::initial_suspend
    //  7 :     calls std::suspend_always::await_ready
    //  8 :     calls std::suspend_always::await_suspend 
    //  9 : creates resumable object
    // 10 : returns


    // 14 : execution restarts     (common prolog gets state from __coro_frame and jumps here)
    std::cout << __FUNCTION__ <<  " 1 \n";

    co_await std::suspend_always();
    // 15 : calls std::suspend_always::await_ready
    // 16 : calls std::suspend_always::await_suspend 
    // 17 : returns


    // 21 : execution restarts     (common prolog gets state from __coro_frame and jumps here)
    std::cout << __FUNCTION__ <<  " 1 \n";

    co_await std::suspend_always();
    // 22 : calls std::suspend_always::await_ready
    // 23 : calls std::suspend_always::await_suspend 
    // 24 : returns

    // 28 : execution restarts     (common prolog gets state from __coro_frame and jumps here)
    std::cout << __FUNCTION__ <<  " 2 \n";

    // 29 : calls resumable::promise_type::return_void
    // 30 : calls resumable::promise_type::final_suspend
    // 31 : sets done flag in _coro_frame
    // 32 : calls std::suspend_always::await_suspend 
    // 33 : returns
}


int main()
{
    std::cout << __FUNCTION__ << "\n";

    resumable res = go();
        // 1 : calls go

    std::cout << __FUNCTION__ << "\n";


    while (res.resume())
        // 11 : calls resumable::resume

        // 18 : calls resumable::resume

        // 25 : calls resumable::resume
    {
        std::cout << __FUNCTION__ << " loop\n";
    }

    // 
}