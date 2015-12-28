#include "stack.h"
#include <iostream>


TStack::TStack(const std::string fileName) : byteCodeGen(new TByteCodeGen(fileName))
{
    std::cout << byteCodeGen->bytecodeString.str();
    DoCode();
    PrintResult();
}

TStack::~TStack()
{
}

void TStack::DoCode()
{
    while (!(byteCodeGen->bytecodeString.eof())) {
        char cmd;
        byteCodeGen->bytecodeString >> cmd;
        switch ((ECommand)cmd) {
        case ECMD_AllOC: {
            byteCodeCMD.reset(new TByteCodeCMDAlloc(&allocator, byteCodeGen->bytecodeString));
            byteCodeCMD->UpdateStack();
            break;
        }
        case ECMD_PUSH: {
            byteCodeCMD.reset(new TByteCodeCMDPush(&stack, allocator, byteCodeGen->bytecodeString));
            byteCodeCMD->UpdateStack();
            break;
        }
        case ECMD_PUSHIDENT: {
            byteCodeCMD.reset(new TByteCodeCMDPushIdent(&stack, defineVar, byteCodeGen->bytecodeString));
            byteCodeCMD->UpdateStack();
            break;
        }
        case ECMD_CALL: {
            byteCodeCMD.reset(new TByteCodeCMDCall(&stack, allocator, defineVar, defineFunc, byteCodeGen->bytecodeString));
            byteCodeCMD->UpdateStack();
            break;
        }
        case ECMD_DEFSTART: {
            byteCodeCMD.reset(new TByteCodeCMDDefine(&defineFunc, byteCodeGen->bytecodeString));
            byteCodeCMD->UpdateStack();
            break;
        }
        case ECMD_IFELSE: {
            byteCodeCMD.reset(new TByteCodeCMDIfElse(&stack, byteCodeGen->bytecodeString));
            byteCodeCMD->UpdateStack();
            break;
        }
        default:
            break;
        }
    }
}

void TStack::PrintResult()
{
    ExprType* expr = stack.at(stack.size()-1);
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

