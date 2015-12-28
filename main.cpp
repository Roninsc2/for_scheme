#include <iostream>
#include "stack.h"

using namespace std;

int main()
{
    try {
        TStack pars("test.txt");
    } catch (Exception* e) {
        std::cerr << e->what() << std::endl;
        delete e;
    }
    return 0;
}

