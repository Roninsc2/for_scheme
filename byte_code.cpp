#include "byte_code.h"


TByteCode::TByteCode(const std::string fileName, const std::string outputFile): Parser(new TParser(fileName))
{
    fout.open(outputFile);
    GetByteCode();
}

TByteCode::~TByteCode() {
    fout.close();
}

void TByteCode::GetByteCode()
{
    for (size_t i = 0; i < Parser->root.size(); i++) {
        switch (Parser->root[i]->Type) {
        case AT_Func: {
            GetFuncByteCode((CallExprAST*)Parser->root[i]);
            break;
        }
        default:
            break;
        }
    }
}

void TByteCode::GetFuncByteCode(CallExprAST* func)
{
    std::string name = func->Callee;
    for (size_t i = 0; i < func->Args.size(); i++) {
        fout << "ADD ";
        GetExprValue(func->Args[i]);
    }
    fout << "DOFUNC " << name << std::endl;
}

void TByteCode::GetExprValue(ExprAST *expr) {
    switch (expr->Type) {
    case AT_Bool: {
        fout << "( 0 " << ((BoolAST*)expr)->value << " )" << std::endl;;
        return;
    }
    case AT_String : {
        fout << "( 1 " << ((StringAST*)expr)->value << " )" << std::endl;;
        return;
    }
    case AT_Symbol : {
        fout << "( 2 "<< ((SymbolAST*)expr)->value << " )" << std::endl;;
        return;
    }
    case AT_Char : {
        fout << "( 3 " << ((CharAST*)expr)->value << " )" << std::endl;;
        return;
    }
    case AT_Int : {
        fout << "( 4 " << ((NumberIntAST*)expr)->value << " )" << std::endl;;
        return;
    }
    case AT_Double : {
        fout  << "( 5 " <<((NumberDoubleAST*)expr)->value << " )" << std::endl;;
        return;
    }
    case AT_Ident : {
        GetExprValue(((IdentAST*)expr)->value);
        return;
    }
    case AT_Func : {
        GetFuncByteCode((CallExprAST*)expr);
    }
    default: {
        break;
        //error
    }
    }
}

