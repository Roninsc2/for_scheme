#include "byte_code_gen.h"


TByteCodeGen::TByteCodeGen(const std::string fileName): Parser(new TParser(fileName))
{
    GeTByteCodeGen();
}

TByteCodeGen::~TByteCodeGen() {
}

void TByteCodeGen::GeTByteCodeGen()
{
    for (size_t i = 0; i < Parser->root.size(); i++) {
        Allocator(Parser->root[i]);
    }
    for (size_t i = 0; i < Parser->root.size(); i++) {
        if (Parser->root[i]->Type != SAT_Define) {
            GetExprValue(Parser->root[i]);
        } else {
            FunctionAST* currentDefine = (FunctionAST*)Parser->root[i];
            bytecodeString << ' ' << (char)CMD_DEFSTART << ' ' << currentDefine->Proto->Name << ' ' << currentDefine->Proto->Args.size() << ' ';
            for (size_t j = 0; j < currentDefine->Proto->Args.size(); j++) {
                bytecodeString << ' ' << currentDefine->Proto->Args[j]->name << '\n';
            }
            for (size_t j = 0; j < currentDefine->Body.size(); j++) {
                GetDefineByteCode(currentDefine->Body[j], currentDefine ->Proto->Name, bytecodeString.tellp());
            }
            bytecodeString << ' ' << (char)CMD_ENDDEF << '\n';
        }
    }
}

void TByteCodeGen::GetFuncByteCode(CallExprAST* func)
{
    bytecodeString << ' ' << (char)CMD_CALL << ' ' << func->Callee << '\n';
    for (size_t i = 0; i < func->Args.size(); i++) {
        GetExprValue(func->Args[i]);
    }
    bytecodeString << ' ' << (char)CMD_ENDCALL << '\n';
}

void TByteCodeGen::GetTaliCallByteCode(CallExprAST *func, std::stringstream::pos_type pos)
{
    bytecodeString << ' ' << (char)CMD_TAILCALL << ' ' << func->Callee << ' ' << ((int)pos - 1) << '\n';
    for (size_t i = 0; i < func->Args.size(); i++) {
        GetExprValue(func->Args[i]);
    }
    bytecodeString << ' ' << (char)CMD_ENDCALL << '\n';
}

void TByteCodeGen::GetIfElseByteCode(IfElseExprAST *expr,  std::stringstream::pos_type pos, std::string name)
{
    if (!name.size()) {
        bytecodeString << ' ' << (char)CMD_IFELSE << '\n';
        GetExprValue(expr->test.get());
        GetExprValue(expr->first.get());
        if (expr->second) {
            GetExprValue(expr->second.get());
        }
        bytecodeString << ' ' << (char)CMD_ENDCALL << '\n';
    } else {
        bytecodeString << ' ' << (char)CMD_IFELSE << ' ';
        GetDefineByteCode(expr->test.get(), name, pos);
        GetDefineByteCode(expr->first.get(), name, pos);
        if (expr->second) {
            GetDefineByteCode(expr->second.get(), name, pos);
        }
        bytecodeString << ' ' << (char)CMD_ENDCALL << '\n';
    }
}

void TByteCodeGen::GetDefineByteCode(ExprAST* expr, std::string name, std::stringstream::pos_type pos)
{
    switch (expr->Type){
    case SAT_IfElse: {
        GetIfElseByteCode((IfElseExprAST*)expr, pos, name);
        return;
    }
    case AT_Func: {
        if (((CallExprAST*)expr)->Callee == name) {
            GetTaliCallByteCode((CallExprAST*)expr, pos);
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

void TByteCodeGen::Allocator(ExprAST* expr)
{
    if (IsInAllocatorValue(expr)) {
        return;
    }
    if (expr->Type != AT_Func && expr->Type != AT_Ident && expr->Type != SAT_Define && expr->Type != SAT_IfElse && !IsInAllocatorValue(expr)){
        allocatorValue.insert(std::make_pair(expr, allocatorValue.size()));
    }
    switch (expr->Type) {
    case AT_Int: {
        bytecodeString << ' ' << (char)CMD_AllOC << ' ' << (char)VT_INT << ' ' << ((NumberIntAST*)expr)->value << '\n';
        return;
    }
    case AT_Double : {
        bytecodeString << ' ' << (char)CMD_AllOC << ' ' << (char)VT_DOUBLE << ' ' << ((NumberDoubleAST*)expr)->value << '\n';
        return;
    }
    case AT_String : {
        bytecodeString << ' ' << (char)CMD_AllOC << ' ' << (char)VT_STRING << ' ' << ((StringAST*)expr)->value << '\n';
        return;
    }
    case AT_Symbol : {
        bytecodeString << ' ' << (char)CMD_AllOC << ' ' << (char)VT_SYMBOL << ' ' << ((SymbolAST*)expr)->value << '\n';
        return;
    }
    case AT_Char : {
        bytecodeString << ' ' << (char)CMD_AllOC << ' ' << (char)VT_CHAR << ' ' << ((CharAST*)expr)->value << '\n';
        return;
    }
    case AT_Bool : {
        bytecodeString << ' ' << (char)CMD_AllOC << ' ' << (char)VT_BOOL << ' ' << ((BoolAST*)expr)->value << '\n';
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

void TByteCodeGen::GetExprValue(ExprAST *expr) {
    switch (expr->Type) {
    case AT_Ident : {
        bytecodeString << ' ' << (char)CMD_PUSHIDENT << ' ' << ((IdentAST*)expr)->name << '\n';
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
        bytecodeString << ' ' << (char)CMD_PUSH << ' ' << GetAllocatorValue(expr) << '\n';
        return;
    }
    }
}

size_t TByteCodeGen::GetAllocatorValue(ExprAST *expr)
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

bool TByteCodeGen::IsInAllocatorValue(ExprAST *expr)
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

