#include "stack.h"
#include <iostream>


TStack::TStack() : currentDefine(&define)
{
}

TStack::~TStack()
{

}

std::shared_ptr<ExprType> TStack::StackPop() {
    std::shared_ptr<ExprType> expr = stack.at(stack.size()-1);
    stack.pop_back();
    return expr;
}

void TStack::StackPushBack(std::shared_ptr<ExprType> expr)
{
    stack.push_back(expr);
}

size_t TStack::GetStackSize()
{
    return stack.size();
}

void TStack::AllocatorPushBack(std::shared_ptr<ExprType> expr)
{
    allocator.push_back(expr);
}

std::shared_ptr<ExprType> TStack::GetAllocatorAt(size_t i)
{
    return allocator.at(i);
}

Enviroment &TStack::GetGlobalEnviroment()
{
    return define;
}

Enviroment* TStack::GetCurrentEnviroment()
{
    return currentDefine;
}

void TStack::ResetCurrentEnviroment(Enviroment* env)
{
        currentDefine = env;
}

void TStack::PrintResult()
{
    for (size_t i = 0; i < stack.size(); i++) {
        ExprType* expr = stack.at(i).get();
        switch (expr->Type) {
        case T_Int: {
            std::cout << dynamic_cast<NumberIntType*>(expr)->GetValue() << std::endl;
            break;
        }
        case T_Double: {
            std::cout << dynamic_cast<NumberDoubleType*>(expr)->GetValue() << std::endl;
            break;
        }
        case T_Symbol: {
            std::cout << dynamic_cast<SymbolType*>(expr)->GetValue() << std::endl;
            break;
        }
        case T_String: {
            std::cout << "\"" << dynamic_cast<StringType*>(expr)->GetValue() << "\"" << std::endl;
            break;
        }
        case T_Char: {
            std::cout << dynamic_cast<CharType*>(expr)->GetValue() << std::endl;
            break;
        }
        case T_Bool: {
            if (dynamic_cast<BoolType*>(expr)->GetValue()) {
                std::cout << "#t" << std::endl;
            } else {
                std::cout << "#f" << std::endl;
            }
            break;
        }
        case T_List: {
            dynamic_cast<ListType*>(expr)->GetValue()->GetListData();
            std::cout << std::endl;
            break;
        }
        default:
            break;
        }
    }
}

