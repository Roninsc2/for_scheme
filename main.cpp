#include <iostream>
#include "lexer.h"
#include "vlist.h"
#include "parser.h"

using namespace std;

int main()
{
    try {
        TParser pars("test.txt");
    } catch (LexerException* e) {
        std::cerr << e->what() << std::endl;
        delete e;
    }
    return 0;
}

