#include <iostream>
#include <coroutine>
#include <thread>
#include <cassert>

#include <mutex>

std::recursive_mutex  ioLock;

namespace jle
{

// https://devblogs.microsoft.com/oldnewthing/20191209-00/?p=103195


struct resume_new_thread : std::suspend_always
{
    void await_suspend(std::coroutine_handle<> handle)   const noexcept
    {
        std::lock_guard _{ioLock};


        std::cout << __FUNCTION__  << "\n";

        std::thread([handle]
        { 
            std::cout << __FUNCTION__  << "\n";
            handle(); 
        }).detach();
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
            std::lock_guard _{ioLock};
            std::cout << __FUNCTION__  << "\n";
        }

        auto get_return_object()                                            // must be called this
        {
            std::lock_guard _{ioLock};
            std::cout << __FUNCTION__  << "\n";
            return coro_handle::from_promise(*this);
        }

        auto initial_suspend()                                              // must be called this
        { 
            std::lock_guard _{ioLock};
            std::cout << __FUNCTION__ << "\n";
            return jle::resume_new_thread(); 
        }

        auto final_suspend() noexcept                                       // must be called this
        { 
            std::lock_guard _{ioLock};
            std::cout << __FUNCTION__ << "\n";
            return jle::resume_new_thread(); 
        }


        void unhandled_exception()                                          // must be called this
        {
//            std::lock_guard _{ioLock};
            std::cout << __FUNCTION__ << "\n";
            std::terminate();
        }


        auto yield_value(int i)                                          // must be called this - called on co_yield
        {
            std::lock_guard _{ioLock};
            std::cout << __FUNCTION__ << ' ' << i << "\n";
            value=i;
            return jle::resume_new_thread(); 
        }

        void return_value(int i)                                          // must be called this - called on co_return
        {
            std::lock_guard _{ioLock};
            std::cout << __FUNCTION__ << ' ' << i << "\n";
            value=i;
        }
    };



    using coro_handle = std::coroutine_handle<promise_type>;


    resumable(coro_handle handle) : handle(handle) 
    { 
        std::lock_guard _{ioLock};
        std::cout << __FUNCTION__ << "\n";
    }

    ~resumable() 
    { 
        std::lock_guard _{ioLock};
        std::cout << __FUNCTION__ << "\n";
        handle.destroy(); 
    }


    resumable(const resumable  &)            = delete;
    resumable(      resumable &&)            = delete;
    resumable &operator=(const resumable  &) = delete;
    resumable &operator=(      resumable &&) = delete;


    bool resume() 
    {
        std::lock_guard _{ioLock};
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
    int pi[]={3,1,4,1,5,9};

    std::cout << __FUNCTION__ "_" << std::this_thread::get_id() << "\n";

    co_await  jle::resume_new_thread{};

    std::cout << __FUNCTION__ "_" << std::this_thread::get_id() << "\n";


    co_return pi[5];
}


int main()
{
    std::cout << __func__ << "\n";

    resumable res = go();

   
    while (res.resume())
    {
        std::lock_guard _{ioLock};
        std::cout << __func__ << " loop " << res.value() << '\n';;
    }

    std::cout << "Final value was " << res.value() << std::endl;
  
  }