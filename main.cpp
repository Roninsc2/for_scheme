#include <iostream>
#include "lexer.h"

using namespace std;

int main()
{
    try {
        TLexer lex("test.txt");
    } catch (LexerException* e) {
        std::cerr << e->what() << std::endl;
        delete e;
    }
    return 0;
}

