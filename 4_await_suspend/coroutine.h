#include <coroutine>
#include <callstack/callstack.h>

namespace jle
{

// this is derived from std::coroutine_handle.   Surprisingly, it works.


template <class Promise>
struct coroutine_handle : public std::coroutine_handle<Promise>
{
    constexpr coroutine_handle() noexcept = default;
    constexpr coroutine_handle(nullptr_t) noexcept {}


    coroutine_handle(std::coroutine_handle<Promise> &&handle) : std::coroutine_handle<Promise>{handle}
    {}


    _NODISCARD static coroutine_handle from_promise(Promise& promise) noexcept { // strengthened

        callstack::printStack();

        return    std::coroutine_handle<Promise>::from_promise(promise) ;
    }


    _NODISCARD static constexpr coroutine_handle from_address(void* const _Addr) noexcept { // strengthened

        callstack::printStack();


        coroutine_handle _Result;
        _Result._Ptr = _Addr;
        return _Result;
    }


    _NODISCARD bool done() const noexcept { // strengthened

        callstack::printStack();

        auto done = std::coroutine_handle<Promise>::done();

        std::cout << "done says " << std::boolalpha << done << "\n";
         
        return done;

    }

    void resume() const {
        callstack::printStack();
        return std::coroutine_handle<Promise>::resume();
    }

    void destroy() const noexcept { // strengthened
        callstack::printStack();
        return std::coroutine_handle<Promise>::destroy();
    }

    _NODISCARD Promise& promise() const noexcept { // strengthened
        
        callstack::printStack();
        
        return std::coroutine_handle<Promise>::promise();
    }

};


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
        callstack::printStack();
        return false;
    }

    void await_suspend(std::coroutine_handle<>) const noexcept 
    {
        callstack::printStack();
    }
    
    void await_resume() const noexcept 
    {
        callstack::printStack();
    }
};

}
