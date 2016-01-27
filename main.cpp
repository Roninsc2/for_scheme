#include <iostream>
#include "byte_code_gen.h"

using namespace std;

int main()
{
    try {
        TByteCodeGen pars("test.txt");
    } catch (Exception* e) {
        std::cerr << e->what() << std::endl;
        delete e;
    }
    return 0;
}

