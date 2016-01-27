#include "byte_code_gen.h"


TByteCodeGen::TByteCodeGen(const std::string fileName): Parser(new TParser(fileName)), Stack(TStack())
{
    GenByteCode();
    while (it < command.size()) {
        command[it]->UpdateStack();
        it++;
    }
    Stack.PrintResult();
}

TByteCodeGen::~TByteCodeGen() {

}

void TByteCodeGen::GenByteCode()
{
    for (size_t i = 0; i < Parser->root.size(); i++) {
        Allocator(Parser->root[i].get());
    }
    for (size_t i = 0; i < Parser->root.size(); i++) {
        if (Parser->root[i]->Type != SAT_Define) {
            GenExprValue(Parser->root[i].get());
        } else {
            FunctionAST* currentDefine = dynamic_cast<FunctionAST*>(Parser->root[i].get());
            std::vector<std::string> args;
            for (size_t j = 0; j < currentDefine->Proto->Args.size(); j++) {
                args.push_back(currentDefine->Proto->Args[j]->name);
            }
            command.push_back(std::shared_ptr<TByteCodeCMD>(new TByteCodeCMDDefine(currentDefine->Proto->Name, currentDefine->Proto->Args.size(), args, Stack, command, it)));
            for (size_t j = 0; j < currentDefine->Body.size(); j++) {
                GenDefineByteCode(currentDefine->Body[j].get(), currentDefine->Proto->Name, command.size());
            }
            command.push_back(std::shared_ptr<TByteCodeCMD>(new TByteCodeCMDEndDef()));
        }
    }
}

void TByteCodeGen::GenFuncByteCode(CallExprAST* func)
{
    command.push_back(std::shared_ptr<TByteCodeCMD>(new TByteCodeCMDCall(func->Callee, Stack, command, it)));
    for (size_t i = 0; i < func->Args.size(); i++) {
        GenExprValue(func->Args[i].get());
    }
    command.push_back(std::shared_ptr<TByteCodeCMD>(new TByteCodeCMDEndCall()));
}

void TByteCodeGen::GenTaliCallByteCode(CallExprAST *func, size_t pos)
{
    command.push_back(std::shared_ptr<TByteCodeCMD>(new TByteCodeCMDTailCall(func->Callee, pos, Stack, command, it)));
    for (size_t i = 0; i < func->Args.size(); i++) {
        GenExprValue(func->Args[i].get());
    }
    command.push_back(std::shared_ptr<TByteCodeCMD>(new TByteCodeCMDEndCall()));
}

void TByteCodeGen::GenIfElseByteCode(IfElseExprAST *expr,  size_t pos, std::string name)
{
    if (!name.size()) {
        command.push_back(std::shared_ptr<TByteCodeCMD>(new TByteCodeCMDIfElse(Stack, command, it)));
        GenExprValue(expr->test.get());
        GenExprValue(expr->first.get());
        if (expr->second) {
            GenExprValue(expr->second.get());
        }
        command.push_back(std::shared_ptr<TByteCodeCMD>(new TByteCodeCMDEndCall()));
    } else {
        command.push_back(std::shared_ptr<TByteCodeCMD>(new TByteCodeCMDIfElse(Stack, command, it)));
        GenDefineByteCode(expr->test.get(), name, pos);
        GenDefineByteCode(expr->first.get(), name, pos);
        if (expr->second) {
            GenDefineByteCode(expr->second.get(), name, pos);
        }
        command.push_back(std::shared_ptr<TByteCodeCMD>(new TByteCodeCMDEndCall()));
    }
}

void TByteCodeGen::GenDefineByteCode(ExprAST* expr, std::string name, size_t pos)
{
    switch (expr->Type){
    case SAT_IfElse: {
        GenIfElseByteCode(dynamic_cast<IfElseExprAST*>(expr), pos, name);
        return;
    }
    case AT_Func: {
        if (dynamic_cast<CallExprAST*>(expr)->Callee == name) {
            GenTaliCallByteCode(dynamic_cast<CallExprAST*>(expr), pos);
        } else {
            GenFuncByteCode(dynamic_cast<CallExprAST*>(expr));
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
        command.push_back(std::shared_ptr<TByteCodeCMD>(new TByteCodeCMDAllocInt(dynamic_cast<NumberIntAST*>(expr)->value, Stack)));
        return;
    }
    case AT_Double : {
        command.push_back(std::shared_ptr<TByteCodeCMD>(new TByteCodeCMDAllocDouble(dynamic_cast<NumberDoubleAST*>(expr)->value, Stack)));
        return;
    }
    case AT_String : {
        command.push_back(std::shared_ptr<TByteCodeCMD>(new TByteCodeCMDAllocString(dynamic_cast<StringAST*>(expr)->value, Stack)));
        return;
    }
    case AT_Symbol : {
        command.push_back(std::shared_ptr<TByteCodeCMD>(new TByteCodeCMDAllocSymbol(dynamic_cast<SymbolAST*>(expr)->value, Stack)));
        return;
    }
    case AT_Char : {
        command.push_back(std::shared_ptr<TByteCodeCMD>(new TByteCodeCMDAllocChar(dynamic_cast<CharAST*>(expr)->value, Stack)));
        return;
    }
    case AT_Bool : {
        command.push_back(std::shared_ptr<TByteCodeCMD>(new TByteCodeCMDAllocBool(dynamic_cast<BoolAST*>(expr)->value, Stack)));
        return;
    }
    case SAT_IfElse: {
        Allocator(dynamic_cast<IfElseExprAST*>(expr)->test.get());
        Allocator(dynamic_cast<IfElseExprAST*>(expr)->first.get());
        if (dynamic_cast<IfElseExprAST*>(expr)->second) {
            Allocator(dynamic_cast<IfElseExprAST*>(expr)->second.get());
        }
        return;
    }
    case AT_Func : {
        size_t size = dynamic_cast<CallExprAST*>(expr)->Args.size();
        for (size_t i = 0; i < size; i++) {
            Allocator(dynamic_cast<CallExprAST*>(expr)->Args[i].get());
        }
        return;
    }
    case SAT_Define: {
        size_t size = dynamic_cast<FunctionAST*>(expr)->Body.size();
        for (size_t i = 0; i < size; i++) {
            Allocator(dynamic_cast<FunctionAST*>(expr)->Body[i].get());
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
        command.push_back(std::shared_ptr<TByteCodeCMD>(new TByteCodeCMDPushIdent(dynamic_cast<IdentAST*>(expr)->name, Stack)));
        return;
    }
    case SAT_IfElse: {
        GenIfElseByteCode(dynamic_cast<IfElseExprAST*>(expr));
        return;
    }
    case AT_Func : {
        GenFuncByteCode(dynamic_cast<CallExprAST*>(expr));
        return;
    }
    default: {
        command.push_back(std::shared_ptr<TByteCodeCMD>(new TByteCodeCMDPush(GenAllocatorValue(expr), Stack)));
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
                if (dynamic_cast<NumberIntAST*>(expr)->value == dynamic_cast<NumberIntAST*>(it->first)->value) {
                    return it->second;
                }
                break;
            }
            case AT_Double: {
                if (dynamic_cast<NumberDoubleAST*>(expr)->value == dynamic_cast<NumberDoubleAST*>(it->first)->value) {
                    return it->second;
                }
                break;
            }
            case AT_Char: {
                if (dynamic_cast<CharAST*>(expr)->value == dynamic_cast<CharAST*>(it->first)->value) {
                    return it->second;
                }
                break;
            }
            case AT_String: {
                if (dynamic_cast<StringAST*>(expr)->value == dynamic_cast<StringAST*>(it->first)->value) {
                    return it->second;
                }
                break;
            }
            case AT_Bool: {
                if (dynamic_cast<BoolAST*>(expr)->value == dynamic_cast<BoolAST*>(it->first)->value) {
                    return it->second;
                }
                break;
            }
            case AT_Symbol: {
                if (dynamic_cast<SymbolAST*>(expr)->value == dynamic_cast<SymbolAST*>(it->first)->value) {
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
                if (dynamic_cast<NumberIntAST*>(expr)->value == dynamic_cast<NumberIntAST*>(it->first)->value) {
                    return true;
                }
                break;
            }
            case AT_Double: {
                if (dynamic_cast<NumberDoubleAST*>(expr)->value == dynamic_cast<NumberDoubleAST*>(it->first)->value) {
                    return true;
                }
                break;
            }
            case AT_Char: {
                if (dynamic_cast<CharAST*>(expr)->value == dynamic_cast<CharAST*>(it->first)->value) {
                    return true;
                }
                break;
            }
            case AT_String: {
                if (dynamic_cast<StringAST*>(expr)->value == dynamic_cast<StringAST*>(it->first)->value) {
                    return true;
                }
                break;
            }
            case AT_Bool: {
                if (dynamic_cast<BoolAST*>(expr)->value == dynamic_cast<BoolAST*>(it->first)->value) {
                    return true;
                }
                break;
            }
            case AT_Symbol: {
                if (dynamic_cast<SymbolAST*>(expr)->value == dynamic_cast<SymbolAST*>(it->first)->value) {
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

