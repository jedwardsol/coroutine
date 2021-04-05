#include <vector>
#include <string>


namespace callstack
{
    void                     init();
    std::vector<std::string> get();
}

#pragma comment(lib,"callstack")
#pragma comment(lib,"dbghelp")
