#include "stack.h"
#include <iostream>


TStack::TStack()
{
}

TStack::~TStack()
{
}

void TStack::PrintResult()
{
    ExprType* expr = stack.at(stack.size()-1).get();
    switch (expr->Type) {
    case T_Int: {
        std::cout << ((NumberIntType*)expr)->value << std::endl;
        break;
    }
    case T_Double: {
        std::cout << ((NumberDoubleType*)expr)->value << std::endl;
        break;
    }
    case T_Symbol: {
        std::cout << ((SymbolType*)expr)->value << std::endl;
        break;
    }
    case T_String: {
        std::cout << ((StringType*)expr)->value << std::endl;
        break;
    }
    case T_Char: {
        std::cout << ((CharType*)expr)->value << std::endl;
        break;
    }
    case T_Bool: {
        std::cout << ((BoolType*)expr)->value << std::endl;
        break;
    }
    case T_List: {
        ((ListType*)expr)->value->GetListData();
        break;
    }
    default:
        break;
    }
}

