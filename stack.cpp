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
        std::cout << dynamic_cast<NumberIntType*>(expr)->value << std::endl;
        break;
    }
    case T_Double: {
        std::cout << dynamic_cast<NumberDoubleType*>(expr)->value << std::endl;
        break;
    }
    case T_Symbol: {
        std::cout << dynamic_cast<SymbolType*>(expr)->value << std::endl;
        break;
    }
    case T_String: {
        std::cout << dynamic_cast<StringType*>(expr)->value << std::endl;
        break;
    }
    case T_Char: {
        std::cout << dynamic_cast<CharType*>(expr)->value << std::endl;
        break;
    }
    case T_Bool: {
        if (dynamic_cast<BoolType*>(expr)->value) {
            std::cout << "#t" << std::endl;
        } else {
            std::cout << "#f" << std::endl;
        }
        break;
    }
    case T_List: {
        dynamic_cast<ListType*>(expr)->value->GetListData();
        std::cout << std::endl;
        break;
    }
    default:
        break;
    }
}

