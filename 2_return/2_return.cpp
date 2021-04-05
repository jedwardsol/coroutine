#include <iostream>
#include <coroutine>
#include <thread>
#include <cassert>

struct resumable
{
    struct promise_type                                                     // must be called this
    {
        using coro_handle = std::coroutine_handle<promise_type>;            

        int value{};            

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


        void unhandled_exception()                                          // must be called this
        {
            std::cout << __FUNCTION__ << "\n";
            std::terminate();
        }


        void return_value(int i)                                          // must be called this
        {
            std::cout << __FUNCTION__ << ' ' << i << "\n";
            value=i;
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
    std::cout << __FUNCTION__ <<  " 1 \n";
    co_await std::suspend_always();
    std::cout << __FUNCTION__ <<  " 2 \n";
    co_await std::suspend_always();
    std::cout << __FUNCTION__ <<  " 3 \n";
    co_return 42;
}


int main()
{
    std::cout << __FUNCTION__ << "\n";

    resumable res = go();

    std::cout << __FUNCTION__ << "\n";


    while (res.resume())
    {
        std::cout << __FUNCTION__ << " loop\n";
    }

    std::cout << "Final value was " << res.value() << std::endl;
  
  }