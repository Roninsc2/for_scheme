#include "byte_code_gen.h"


TByteCodeGen::TByteCodeGen(const std::string fileName): Parser(new TParser(fileName))
{
    GenByteCode();
}

TByteCodeGen::~TByteCodeGen() {
}

void TByteCodeGen::GenByteCode()
{
    for (size_t i = 0; i < Parser->root.size(); i++) {
        Allocator(Parser->root[i]);
    }
    for (size_t i = 0; i < Parser->root.size(); i++) {
        if (Parser->root[i]->Type != SAT_Define) {
            GenExprValue(Parser->root[i]);
        } else {
            FunctionAST* currentDefine = (FunctionAST*)Parser->root[i];
            std::vector<std::string> args;
            for (size_t j = 0; j < currentDefine->Proto->Args.size(); j++) {
                args.push_back(currentDefine->Proto->Args[j]->name);
            }
            command.push_back(new TByteCodeCMDDefine(currentDefine->Proto->Name, currentDefine->Proto->Args.size(), args));
            for (size_t j = 0; j < currentDefine->Body.size(); j++) {
                GenDefineByteCode(currentDefine->Body[j], currentDefine ->Proto->Name, command.size());
            }
            command.push_back(new TByteCodeCMDEndDef());
        }
    }
}

void TByteCodeGen::GenFuncByteCode(CallExprAST* func)
{
    command.push_back(new TByteCodeCMDCall(func->Callee));
    for (size_t i = 0; i < func->Args.size(); i++) {
        GenExprValue(func->Args[i]);
    }
    command.push_back(new TByteCodeCMDEndCall());
}

void TByteCodeGen::GenTaliCallByteCode(CallExprAST *func, size_t pos)
{
    command.push_back(new TByteCodeCMDTailCall(func->Callee, pos));
    for (size_t i = 0; i < func->Args.size(); i++) {
        GenExprValue(func->Args[i]);
    }
    command.push_back(new TByteCodeCMDEndCall());
}

void TByteCodeGen::GenIfElseByteCode(IfElseExprAST *expr,  size_t pos, std::string name)
{
    if (!name.size()) {
        //bytecodeString << ' ' << (char)CMD_IFELSE << '\n';
        GenExprValue(expr->test.get());
        GenExprValue(expr->first.get());
        if (expr->second) {
            GenExprValue(expr->second.get());
        }
        command.push_back(new TByteCodeCMDEndCall());
    } else {
        //bytecodeString << ' ' << (char)CMD_IFELSE << ' ';
        GenDefineByteCode(expr->test.get(), name, pos);
        GenDefineByteCode(expr->first.get(), name, pos);
        if (expr->second) {
            GenDefineByteCode(expr->second.get(), name, pos);
        }
        command.push_back(new TByteCodeCMDEndCall());
    }
}

void TByteCodeGen::GenDefineByteCode(ExprAST* expr, std::string name, size_t pos)
{
    switch (expr->Type){
    case SAT_IfElse: {
        GenIfElseByteCode((IfElseExprAST*)expr, pos, name);
        return;
    }
    case AT_Func: {
        if (((CallExprAST*)expr)->Callee == name) {
            GenTaliCallByteCode((CallExprAST*)expr, pos);
        } else {
            GenFuncByteCode((CallExprAST*)expr);
        }
        return;
    }
    default:
        GenExprValue(expr);
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
        command.push_back(new TByteCodeCMDAllocInt(((NumberIntAST*)expr)->value));
        return;
    }
    case AT_Double : {
        command.push_back(new TByteCodeCMDAllocDouble(((NumberDoubleAST*)expr)->value));
        return;
    }
    case AT_String : {
        command.push_back(new TByteCodeCMDAllocString(((StringAST*)expr)->value));
        return;
    }
    case AT_Symbol : {
        command.push_back(new TByteCodeCMDAllocSymbol(((SymbolAST*)expr)->value));
        return;
    }
    case AT_Char : {
        command.push_back(new TByteCodeCMDAllocChar(((CharAST*)expr)->value));
        return;
    }
    case AT_Bool : {
        command.push_back(new TByteCodeCMDAllocBool(((BoolAST*)expr)->value));
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

void TByteCodeGen::GenExprValue(ExprAST *expr) {
    switch (expr->Type) {
    case AT_Ident : {
        command.push_back(new TByteCodeCMDPushIdent(((IdentAST*)expr)->name));
        return;
    }
    case SAT_IfElse: {
        GenIfElseByteCode((IfElseExprAST*)expr);
        return;
    }
    case AT_Func : {
        GenFuncByteCode((CallExprAST*)expr);
        return;
    }
    default: {
        command.push_back(new TByteCodeCMDPush(GenAllocatorValue(expr)));
        return;
    }
    }
}

size_t TByteCodeGen::GenAllocatorValue(ExprAST *expr)
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

