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
    for (size_t i = 0; i < Parser->define.size(); i++) {
        fout << "DEFINE " << Parser->define[i]->Proto->Name << " ";
        for (size_t j = 0; j < Parser->define[i]->Proto->Args.size(); j++) {
            fout << ((IdentAST*)Parser->define[i]->Proto->Args[j])->name << " ";
        }
        fout << std::endl;
        for (size_t j = 0; j < Parser->define[i]->Body.size(); j++) {
            switch (Parser->define[i]->Body[j]->Type) {
            case AT_Func:
                GetFuncByteCode((CallExprAST*)Parser->define[i]->Body[j], 0);
                break;
            default:
                GetExprValue(Parser->define[i]->Body[j], 0);
                break;
            }
        }
    }
    fout << "ENDDEFINE " << std::endl;
    for (size_t i = 0; i < Parser->root.size(); i++) {
        switch (Parser->root[i]->Type) {
        case AT_Func: {
            GetFuncByteCode((CallExprAST*)Parser->root[i], 0);
            break;
        }
        default:
            GetExprValue(Parser->root[i],0);
            break;
        }
    }
}

void TByteCode::GetFuncByteCode(CallExprAST* func, size_t level)
{
    std::string name = func->Callee;
    for (size_t i = 0; i < func->Args.size(); i++) {
        GetExprValue(func->Args[i], level);
    }
    fout << "DOFUNC " << level << " " << name << std::endl;
}

void TByteCode::GetExprValue(ExprAST *expr, size_t level) {
    switch (expr->Type) {
    case AT_Int: {
        fout << "ADD " << level << " 0 " << ((NumberIntAST*)expr)->value << std::endl;;
        return;
    }
    case AT_Double : {
        fout << "ADD " << level << " 1 " << ((NumberDoubleAST*)expr)->value  << std::endl;;
        return;
    }
    case AT_String : {
        fout << "ADD " << level << " 2 "<< ((StringAST*)expr)->value  << std::endl;;
        return;
    }
    case AT_Symbol : {
        fout << "ADD " << level << " 3 " << ((SymbolAST*)expr)->value  << std::endl;;
        return;
    }
    case AT_Char : {
        fout << "ADD " << level << " 4 " << ((CharAST*)expr)->value  << std::endl;;
        return;
    }
    case AT_Bool : {
        fout  << "ADD " << level << " 5 " <<((BoolAST*)expr)->value  << std::endl;;
        return;
    }
    case AT_Ident : {
        fout << "ADD " << level << " 7 " << ((IdentAST*)expr)->name << std::endl;
        return;
    }
    case AT_Func : {
        GetFuncByteCode((CallExprAST*)expr, level+1);
    }
    default: {
        break;
        throw new ParserExceptionIncorrectToken("cannot identify token type");
    }
    }
}

