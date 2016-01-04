#include "stack.h"
#include <iostream>


TStack::TStack(const std::string fileName) : byteCodeGen(new TByteCodeGen(fileName))
{
    DoCode();
    PrintResult();
}

TStack::~TStack()
{
}



void TStack::DoCode()
{
    while (it < byteCodeGen->command.size()) {
        switch (byteCodeGen->command[it]->Type) {
            case ECMD_AllOCINT: {
                dynamic_cast<TByteCodeCMDAllocInt*>(byteCodeGen->command[it])->UpdateStack(allocator);
                break;
            }
            case ECMD_AllOCDOUBLE: {
                dynamic_cast<TByteCodeCMDAllocDouble*>(byteCodeGen->command[it])->UpdateStack(allocator);
                break;
            }
            case ECMD_AllOCCHAR: {
                dynamic_cast<TByteCodeCMDAllocChar*>(byteCodeGen->command[it])->UpdateStack(allocator);
                break;
            }
            case ECMD_AllOCSTRING: {
                dynamic_cast<TByteCodeCMDAllocString*>(byteCodeGen->command[it])->UpdateStack(allocator);
                break;
            }
            case ECMD_AllOCSYMBOL: {
                dynamic_cast<TByteCodeCMDAllocSymbol*>(byteCodeGen->command[it])->UpdateStack(allocator);
                break;
            }
            case ECMD_AllOCBOOL: {
                dynamic_cast<TByteCodeCMDAllocBool*>(byteCodeGen->command[it])->UpdateStack(allocator);
                break;
            }
            case ECMD_PUSH: {
                dynamic_cast<TByteCodeCMDPush*>(byteCodeGen->command[it])->UpdateStack(stack, allocator);
                break;
            }
            case ECMD_PUSHIDENT: {
                dynamic_cast<TByteCodeCMDPushIdent*>(byteCodeGen->command[it])->UpdateStack(stack, defineVar);
                break;
            }
            case ECMD_CALL: {
                dynamic_cast<TByteCodeCMDCall*>(byteCodeGen->command[it])->UpdateStack(stack, allocator, defineVar, defineFunc, byteCodeGen->command, it);
                break;
            }
            case ECMD_DEFSTART: {
                dynamic_cast<TByteCodeCMDDefine*>(byteCodeGen->command[it])->UpdateStack(defineFunc, byteCodeGen->command, it);
                break;
            }
            case ECMD_IFELSE: {
                /*byteCodeCMD.reset(new TByteCodeCMDIfElse(&stack, byteCodeGen->command, it));
                byteCodeCMD->UpdateStack();*/
                break;
            }
            default:
                break;
        }
        it++;
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

