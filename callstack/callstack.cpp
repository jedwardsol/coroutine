
#include "callstack.h"
#include <cstdint>
#include <iostream>

#include <Windows.h>
#include <dbghelp.h>
#include <intrin.h>



namespace callstack
{

HANDLE  thisProcess{};


void init()
{

    thisProcess = OpenProcess(PROCESS_ALL_ACCESS,FALSE,GetCurrentProcessId());

    if(!SymInitialize(thisProcess,"srv*;.",TRUE))
    {
        std::cout << "SymInitialize failed " << GetLastError() << "\n";
    }
}



std::string symbol(DWORD64     address)
{
    char symbolBuffer[sizeof(IMAGEHLP_SYMBOL) + 255];

    PIMAGEHLP_SYMBOL symbol = (PIMAGEHLP_SYMBOL)symbolBuffer;

    symbol->SizeOfStruct = (sizeof IMAGEHLP_SYMBOL) + 255;
    symbol->MaxNameLength = 254;

    if (SymGetSymFromAddr(thisProcess, address, NULL, symbol))
    {
        return symbol->Name;
    }
    else
    {
        return {};
    }
}


std::vector<std::string> get()
{
    std::vector<std::string> stack;

    CONTEXT      context{};
    context.ContextFlags = CONTEXT_FULL;
    RtlCaptureContext(&context);    


    STACKFRAME_EX frame{};
    frame.AddrPC        = {context.Rip,0,AddrModeFlat};
    frame.AddrStack     = {context.Rsp,0,AddrModeFlat};
    frame.AddrFrame     = {context.Rbp,0,AddrModeFlat};
    frame.AddrReturn    = {reinterpret_cast<DWORD64>(_ReturnAddress()),0,AddrModeFlat};

    while(StackWalkEx(IMAGE_FILE_MACHINE_AMD64,
                      thisProcess,
                      GetCurrentThread(),
                      &frame,
                      &context,
                      nullptr,
                      SymFunctionTableAccess64 ,
                      SymGetModuleBase64 ,
                      nullptr,
                      SYM_STKWALK_DEFAULT))
    {
        auto symbol= callstack::symbol(frame.AddrPC.Offset);

        stack.push_back(symbol);

        if(symbol=="main")
        {
            break;
        }
    }


    if(!stack.empty())
    {
        if(stack.front() == "callstack::get")
        {
            stack.erase(stack.begin());
        }
    }    

    return stack;
}


}