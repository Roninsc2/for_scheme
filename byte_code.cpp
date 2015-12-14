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
            SaveMany(fout, (char)CMD_DEFSTART, currentDefine->Proto->Name, currentDefine->Proto->Args.size());
            for (size_t j = 0; j < currentDefine->Proto->Args.size(); j++) {
                SaveMany(fout, currentDefine->Proto->Args[j]->name);
            }
            for (size_t j = 0; j < currentDefine->Body.size(); j++) {
                GetDefineByteCode(currentDefine->Body[j], currentDefine ->Proto->Name);
            }
            SaveMany(fout, (char)CMD_ENDDEF);
        }
    }
}

void TByteCode::GetFuncByteCode(CallExprAST* func)
{
    std::string name = func->Callee;
    SaveMany(fout, (char)CMD_CALL, name);
    for (size_t i = 0; i < func->Args.size(); i++) {
        GetExprValue(func->Args[i]);
    }
    SaveMany(fout, (char)CMD_ENDCALL);
}

void TByteCode::GetTaliCallByteCode(CallExprAST *func)
{
    std::string name = func->Callee;
    SaveMany(fout, (char)CMD_TAILCALL, name);
    for (size_t i = 0; i < func->Args.size(); i++) {
        GetExprValue(func->Args[i]);
    }
    SaveMany(fout, (char)CMD_ENDCALL);
}

void TByteCode::GetIfElseByteCode(IfElseExprAST *expr, std::string name)
{
    if (!name.size()) {
        SaveMany(fout, (char)CMD_IFELSE);
        GetExprValue(expr->test.get());
        GetExprValue(expr->first.get());
        if (expr->second) {
            GetExprValue(expr->second.get());
        }
        SaveMany(fout, (char)CMD_ENDCALL);
    } else {
        SaveMany(fout, (char)CMD_IFELSE);
        GetDefineByteCode(expr->test.get(), name);
        GetDefineByteCode(expr->first.get(), name);
        if (expr->second) {
            GetDefineByteCode(expr->second.get(), name);
        }
        SaveMany(fout, (char)CMD_ENDCALL);
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
    if (IsInAllocatorValue(expr)) {
        return;
    }
    if (expr->Type != AT_Func && expr->Type != AT_Ident && expr->Type != SAT_Define && expr->Type != SAT_IfElse && !IsInAllocatorValue(expr)){
        allocatorValue.insert(std::make_pair(expr, allocatorValue.size()));
    }
    switch (expr->Type) {
    case AT_Int: {
        SaveMany(fout, (char)CMD_AllOC, (char)VT_INT, ((NumberIntAST*)expr)->value);
        return;
    }
    case AT_Double : {
        SaveMany(fout, (char)CMD_AllOC, (char)VT_DOUBLE,((NumberDoubleAST*)expr)->value);
        return;
    }
    case AT_String : {
        SaveMany(fout, (char)CMD_AllOC, (char)VT_STRING, ((StringAST*)expr)->value);
        return;
    }
    case AT_Symbol : {
        SaveMany(fout, (char)CMD_AllOC, (char)VT_SYMBOL, ((SymbolAST*)expr)->value);
        return;
    }
    case AT_Char : {
        SaveMany(fout, (char)CMD_AllOC, (char)VT_CHAR, ((CharAST*)expr)->value);
        return;
    }
    case AT_Bool : {
        SaveMany(fout, (char)CMD_AllOC, (char)VT_BOOL, ((BoolAST*)expr)->value);
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
    case AT_Ident : {
        SaveMany(fout, (char)CMD_PUSHIDENT, ((IdentAST*)expr)->name);
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
        SaveMany(fout, (char)CMD_PUSH, GetAllocatorValue(expr));
        return;
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

