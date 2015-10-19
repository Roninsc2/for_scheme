#include <iostream>
#include "lexer.h"
#include "vlist.h"

using namespace std;

int main()
{
    try {
        TLexer lex("test.txt");
    } catch (LexerException* e) {
        std::cerr << e->what() << std::endl;
        delete e;
    }
    VList k(TListTypePtr(new TListTypeInt(1)));
    VList b(TListTypePtr(new TListTypeDouble(2.3)));
    k.InsertAfter(b);
    std::cout << k.ListLength() << std::endl;
    return 0;
}

