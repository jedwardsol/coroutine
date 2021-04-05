#pragma once

#include <vector>
#include <string>
#include <iostream>

namespace callstack
{
    void                     init();
    std::vector<std::string> get();

void inline __forceinline printStack()
{
    auto stack = callstack::get();

    std::reverse(stack.begin(),stack.end());

    if(stack.back() == "callstack::printStack")
    {
        stack.pop_back();
    }


    for(const auto &frame : stack)
    {
        std::cout << " " << frame ;
    }

    std::cout << "\n";
}


}

#pragma comment(lib,"callstack")
#pragma comment(lib,"dbghelp")

