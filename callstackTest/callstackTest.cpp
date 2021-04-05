#include <iostream>
#include "callstack/callstack.h"


void b()
{
    auto callstack = callstack::get();

    for(const auto &frame : callstack)
    {
        std::cout << frame << "\n";
    }
}


void a()
{
    b();
}

int main()
{
    callstack::init();

    a();
}