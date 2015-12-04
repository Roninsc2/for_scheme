#include <iostream>
#include "byte_code.h"

using namespace std;

int main()
{
    try {
        TByteCode pars("test.txt", "out.bin");
    } catch (Exception* e) {
        std::cerr << e->what() << std::endl;
        delete e;
    }
    return 0;
}

