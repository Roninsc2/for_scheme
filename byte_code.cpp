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
        Allocator(Parser->root[i]);
    }
    for (size_t i = 0; i < Parser->root.size(); i++) {
        if (Parser->root[i]->Type != SAT_Define) {
            GetExprValue(Parser->root[i]);
        } else {
            FunctionAST* currentDefine = (FunctionAST*)Parser->root[i];
            fout << "6 " << currentDefine ->Proto->Name << " ";
            for (size_t j = 0; j < currentDefine ->Proto->Args.size(); j++) {
                fout << currentDefine ->Proto->Args[j]->name << " ";
            }
            fout << std::endl;
            for (size_t j = 0; j < currentDefine ->Body.size(); j++) {
                GetDefineByteCode(currentDefine ->Body[j], currentDefine ->Proto->Name);
            }
            fout << "7 " << std::endl;
        }
    }
}

void TByteCode::GetFuncByteCode(CallExprAST* func)
{
    std::string name = func->Callee;
    fout << "3 " << name << std::endl;
    for (size_t i = 0; i < func->Args.size(); i++) {
        GetExprValue(func->Args[i]);
    }
    fout << "5 " << std::endl;
}

void TByteCode::GetTaliCallByteCode(CallExprAST *func)
{
    std::string name = func->Callee;
    fout << "4 " << name << std::endl;
    for (size_t i = 0; i < func->Args.size(); i++) {
        GetExprValue(func->Args[i]);
    }
    fout << "5 " << std::endl;
}

void TByteCode::GetIfElseByteCode(IfElseExprAST *expr, std::string name)
{
    if (!name.size()) {
        fout << "8 " << std::endl;
        GetExprValue(expr->test.get());
        GetExprValue(expr->first.get());
        if (expr->second) {
            GetExprValue(expr->second.get());
        }
        fout << "5 " << std::endl;
    } else {
        fout << "8 " << std::endl;
        GetDefineByteCode(expr->test.get(), name);
        GetDefineByteCode(expr->first.get(), name);
        if (expr->second) {
            GetDefineByteCode(expr->second.get(), name);
        }
        fout << "5 " << std::endl;
    }
}

void TByteCode::GetDefineByteCode(ExprAST* expr, std::string name)
{
    switch (expr->Type){
    case SAT_IfElse: {
        GetIfElseByteCode((IfElseExprAST*)expr, name);
        return;
    }
    case AT_Func: {
        if (((CallExprAST*)expr)->Callee == name) {
            GetTaliCallByteCode((CallExprAST*)expr);
        } else {
            GetFuncByteCode((CallExprAST*)expr);
        }
        return;
    }
    default:
        GetExprValue(expr);
        return;
    }
}

void TByteCode::Allocator(ExprAST* expr)
{
    if (expr->Type != AT_Func && expr->Type != AT_Ident && expr->Type != SAT_Define && expr->Type != SAT_IfElse && !IsInAllocatorValue(expr)){
        allocatorValue.insert(std::make_pair(expr, allocatorValue.size()));
    } else if (IsInAllocatorValue(expr)) {
        return;
    }
    switch (expr->Type) {
    case AT_Int: {
        fout << "0 0 " << ((NumberIntAST*)expr)->value << std::endl;;
        return;
    }
    case AT_Double : {
        fout << "0 1 " << ((NumberDoubleAST*)expr)->value  << std::endl;;
        return;
    }
    case AT_String : {
        fout << "0 2 " << ((StringAST*)expr)->value.length() << " " << ((StringAST*)expr)->value  << std::endl;;
        return;
    }
    case AT_Symbol : {
        fout << "0 3 " << ((StringAST*)expr)->value.length() << " " << ((SymbolAST*)expr)->value  << std::endl;;
        return;
    }
    case AT_Char : {
        fout << "0 4 " << ((CharAST*)expr)->value  << std::endl;;
        return;
    }
    case AT_Bool : {
        fout  << "0 5 " <<((BoolAST*)expr)->value  << std::endl;;
        return;
    }
    case SAT_IfElse: {
        Allocator(((IfElseExprAST*)expr)->test.get());
        Allocator(((IfElseExprAST*)expr)->first.get());
        if (((IfElseExprAST*)expr)->second) {
            Allocator(((IfElseExprAST*)expr)->second.get());
        }
        return;
    }
    case AT_Func : {
        for (size_t i = 0; i < ((CallExprAST*)expr)->Args.size(); i++) {
            Allocator(((CallExprAST*)expr)->Args[i]);
        }
        return;
    }
    case SAT_Define: {
        for (size_t i = 0; i < ((FunctionAST*)expr)->Body.size(); i++) {
             Allocator(((FunctionAST*)expr)->Body[i]);
        }
        return;
    }
    default: {
        break;
        throw new ParserExceptionIncorrectToken("cannot identify token type");
    }
    }
}

void TByteCode::GetExprValue(ExprAST *expr) {
    switch (expr->Type) {
    case AT_Int: {
        fout << "1 " << GetAllocatorValue(expr) << std::endl;;
        return;
    }
    case AT_Double : {
        fout << "1 " << GetAllocatorValue(expr)  << std::endl;;
        return;
    }
    case AT_String : {
        fout << "1 "<< GetAllocatorValue(expr)  << std::endl;;
        return;
    }
    case AT_Symbol : {
        fout << "1 " << GetAllocatorValue(expr)  << std::endl;;
        return;
    }
    case AT_Char : {
        fout << "1 " << GetAllocatorValue(expr)  << std::endl;;
        return;
    }
    case AT_Bool : {
        fout  << "1 " << GetAllocatorValue(expr)  << std::endl;;
        return;
    }
    case AT_Ident : {
        fout << "2 " << ((IdentAST*)expr)->name << std::endl;
        return;
    }
    case SAT_IfElse: {
        GetIfElseByteCode((IfElseExprAST*)expr);
        return;
    }
    case AT_Func : {
        GetFuncByteCode((CallExprAST*)expr);
        return;
    }
    default: {
        break;
        throw new ParserExceptionIncorrectToken("cannot identify token type");
    }
    }
}

size_t TByteCode::GetAllocatorValue(ExprAST *expr)
{
    for (auto it = allocatorValue.begin(); it != allocatorValue.end(); it++) {
        if (expr->Type == it->first->Type) {
            switch (expr->Type) {
            case AT_Int: {
                if (((NumberIntAST*)expr)->value == ((NumberIntAST*)it->first)->value) {
                    return it->second;
                }
                break;
            }
            case AT_Double: {
                if (((NumberDoubleAST*)expr)->value == ((NumberDoubleAST*)it->first)->value) {
                    return it->second;
                }
                break;
            }
            case AT_Char: {
                if (((CharAST*)expr)->value == ((CharAST*)it->first)->value) {
                    return it->second;
                }
                break;
            }
            case AT_String: {
                if (((StringAST*)expr)->value == ((StringAST*)it->first)->value) {
                    return it->second;
                }
                break;
            }
            case AT_Bool: {
                if (((BoolAST*)expr)->value == ((BoolAST*)it->first)->value) {
                    return it->second;
                }
                break;
            }
            case AT_Symbol: {
                if (((SymbolAST*)expr)->value == ((SymbolAST*)it->first)->value) {
                    return it->second;
                }
                break;
            }
            default: {
                break;
            }
            }
        }
    }
}

bool TByteCode::IsInAllocatorValue(ExprAST *expr)
{
    for (auto it = allocatorValue.begin(); it != allocatorValue.end(); it++) {
        if (expr->Type == it->first->Type) {
            switch (expr->Type) {
            case AT_Int: {
                if (((NumberIntAST*)expr)->value == ((NumberIntAST*)it->first)->value) {
                    return true;
                }
                break;
            }
            case AT_Double: {
                if (((NumberDoubleAST*)expr)->value == ((NumberDoubleAST*)it->first)->value) {
                    return true;
                }
                break;
            }
            case AT_Char: {
                if (((CharAST*)expr)->value == ((CharAST*)it->first)->value) {
                    return true;
                }
                break;
            }
            case AT_String: {
                if (((StringAST*)expr)->value == ((StringAST*)it->first)->value) {
                    return true;
                }
                break;
            }
            case AT_Bool: {
                if (((BoolAST*)expr)->value == ((BoolAST*)it->first)->value) {
                    return true;
                }
                break;
            }
            case AT_Symbol: {
                if (((SymbolAST*)expr)->value == ((SymbolAST*)it->first)->value) {
                    return true;
                }
                break;
            }
            default: {
                break;
            }
            }
        }
    }
    return false;
}

