#include "byte_code_gen.h"


TByteCodeGen::TByteCodeGen(const std::string fileName): Parser(new TParser(fileName)), Stack(TStack())
{
    GenByteCode();
    while (it < command.size()) {
        command[it]->UpdateStack(Stack);
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
        GenExprValue(Parser->root[i].get(), Stack.define);
    }
}

void TByteCodeGen::GenFuncByteCode(CallExprAST* func,  Enviroment& defineParent)
{
    for (size_t i = 0; i < func->Args.size(); i++) {
        GenExprValue(func->Args[i].get(), defineParent);
    }
    command.push_back(std::shared_ptr<TByteCodeCMD>(new TByteCodeCMDCall(func->Callee, func->Args.size(), it, command, defineParent)));
}


void TByteCodeGen::GenTaliCallByteCode(CallExprAST *func, Enviroment& defineParent, size_t pos)
{
    for (size_t i = 0; i < func->Args.size(); i++) {
        GenExprValue(func->Args[i].get(), defineParent);
    }
    command.push_back(std::shared_ptr<TByteCodeCMD>(new TByteCodeCMDTailCall(func->Callee, pos, func->Args.size(), it, defineParent)));
}

void TByteCodeGen::GenIfElseByteCode(IfElseExprAST *expr,  Enviroment& defineParent, size_t pos, std::string name)
{
    if (!name.size()) {
            GenExprValue(expr->test.get(), defineParent);
            command.push_back(std::shared_ptr<TByteCodeCMD>(new TByteCodeCMDIfElse(it)));
            command.push_back(std::shared_ptr<TByteCodeCMD>(new TByteCodeCMDJump(it)));
            size_t k = command.size()-1;
            GenExprValue(expr->first.get(), defineParent);
            if (expr->second) {
                dynamic_cast<TByteCodeCMDJump*>(command.at(k).get())->currentPos = command.size();
                command.push_back(std::shared_ptr<TByteCodeCMD>(new TByteCodeCMDJump(it)));
                k = command.size()-1;
                GenExprValue(expr->second.get(), defineParent);
                dynamic_cast<TByteCodeCMDJump*>(command.at(k).get())->currentPos = command.size()-1;
            } else {
                dynamic_cast<TByteCodeCMDJump*>(command.at(k).get())->currentPos = command.size();
            }
        } else {
            GenDefineByteCode(expr->test.get(), defineParent, name, pos);
            command.push_back(std::shared_ptr<TByteCodeCMD>(new TByteCodeCMDIfElse(it)));
            command.push_back(std::shared_ptr<TByteCodeCMD>(new TByteCodeCMDJump(it)));
            size_t k = command.size()-1;
            GenDefineByteCode(expr->first.get(), defineParent, name, pos);
            if (expr->second) {
                dynamic_cast<TByteCodeCMDJump*>(command.at(k).get())->currentPos = command.size();
                command.push_back(std::shared_ptr<TByteCodeCMD>(new TByteCodeCMDJump(it)));
                k = command.size()-1;
                GenDefineByteCode(expr->second.get(), defineParent, name, pos);
                dynamic_cast<TByteCodeCMDJump*>(command.at(k).get())->currentPos = command.size()-1;
            } else {
                dynamic_cast<TByteCodeCMDJump*>(command.at(k).get())->currentPos = command.size();
            }
    }
}

void TByteCodeGen::GenLambdaByteCode(LambdaExprAST *expr, Enviroment& defineParent,  size_t pos, std::string name)
{
    std::vector<std::string> args;
    for (size_t j = 0; j < expr->Idents.size(); j++) {
        args.push_back(expr->Idents[j]->name);
    }
    for (size_t i = 0; i < expr->Args.size(); i++) {
        GenExprValue(expr->Args.at(i).get(), defineParent);
    }
    command.push_back(std::shared_ptr<TByteCodeCMD>(new TByteCodeCMDLambda(args, command)));
    if (!name.size()) {
        for (size_t i = 0; i < expr->Body.size(); i++) {
            GenExprValue(expr->Body[i].get(), defineParent);
        }
    } else {
        for (size_t i = 0; i < expr->Body.size(); i++) {
            GenDefineByteCode(expr->Body[i].get(), defineParent, name, pos);
        }
    }
}

void TByteCodeGen::GenDefineByteCode(ExprAST* expr, Enviroment& defineParent, std::string name, size_t pos)
{
    switch (expr->Type){
    case SAT_IfElse: {
        GenIfElseByteCode(dynamic_cast<IfElseExprAST*>(expr), defineParent, pos, name);
        return;
    }
    case SAT_Lambda: {
        GenLambdaByteCode(dynamic_cast<LambdaExprAST*>(expr), defineParent, pos, name);
        return;
    }
    case SAT_Define: {
        GenDefine(expr, defineParent);
        return;
    }
    case AT_Func: {
        if (dynamic_cast<CallExprAST*>(expr)->Callee == name) {
            GenTaliCallByteCode(dynamic_cast<CallExprAST*>(expr), defineParent, pos);
        } else {
            GenFuncByteCode(dynamic_cast<CallExprAST*>(expr), defineParent);
        }
        return;
    }
    default:
        GenExprValue(expr, defineParent);
        return;
    }
}

void TByteCodeGen::GenDefine(ExprAST *expr, Enviroment& defineParent)
{
    FunctionAST* currentDefine = dynamic_cast<FunctionAST*>(expr);
    std::vector<std::string> args;
    for (size_t j = 0; j < currentDefine->Proto->Args.size(); j++) {
        args.push_back(currentDefine->Proto->Args[j]->name);
    }
    std::shared_ptr<TByteCodeCMDDefine> Define(new TByteCodeCMDDefine(currentDefine->Proto->Name, args, defineParent, command, it));
    command.push_back(Define);
    for (size_t j = 0; j < currentDefine->Body.size(); j++) {
        GenDefineByteCode(currentDefine->Body[j].get(), *(Define->define.get()), currentDefine->Proto->Name, command.size());
    }
    command.push_back(std::shared_ptr<TByteCodeCMD>(new TByteCodeCMDJumpEndDefine(it)));
}

void TByteCodeGen::Allocator(ExprAST* expr)
{
    if (IsInAllocatorValue(expr)) {
        return;
    }
    if (expr->Type != AT_Func && expr->Type != AT_Ident && expr->Type != SAT_Define && expr->Type != SAT_IfElse && expr->Type != SAT_Lambda && !IsInAllocatorValue(expr)){
        allocatorValue.insert(std::make_pair(expr, allocatorValue.size()));
    }
    switch (expr->Type) {
    case AT_Int: {
        command.push_back(std::shared_ptr<TByteCodeCMD>(new TByteCodeCMDAllocInt(dynamic_cast<NumberIntAST*>(expr)->value)));
        return;
    }
    case AT_Double : {
        command.push_back(std::shared_ptr<TByteCodeCMD>(new TByteCodeCMDAllocDouble(dynamic_cast<NumberDoubleAST*>(expr)->value)));
        return;
    }
    case AT_String : {
        command.push_back(std::shared_ptr<TByteCodeCMD>(new TByteCodeCMDAllocString(dynamic_cast<StringAST*>(expr)->value)));
        return;
    }
    case AT_Symbol : {
        command.push_back(std::shared_ptr<TByteCodeCMD>(new TByteCodeCMDAllocSymbol(dynamic_cast<SymbolAST*>(expr)->value)));
        return;
    }
    case AT_Char : {
        command.push_back(std::shared_ptr<TByteCodeCMD>(new TByteCodeCMDAllocChar(dynamic_cast<CharAST*>(expr)->value)));
        return;
    }
    case AT_Bool : {
        command.push_back(std::shared_ptr<TByteCodeCMD>(new TByteCodeCMDAllocBool(dynamic_cast<BoolAST*>(expr)->value)));
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
    case SAT_Lambda: {
        size_t sizeArgs = dynamic_cast<LambdaExprAST*>(expr)->Args.size();
        size_t sizeBody = dynamic_cast<LambdaExprAST*>(expr)->Body.size();
        for (size_t i = 0; i < sizeArgs; i++) {
            Allocator(dynamic_cast<LambdaExprAST*>(expr)->Args[i].get());
        }
        for (size_t i = 0; i < sizeBody; i++) {
            Allocator(dynamic_cast<LambdaExprAST*>(expr)->Body[i].get());
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

void TByteCodeGen::GenExprValue(ExprAST *expr, Enviroment& defineParent) {
    switch (expr->Type) {
    case AT_Ident : {
        command.push_back(std::shared_ptr<TByteCodeCMD>(new TByteCodeCMDPushIdent(dynamic_cast<IdentAST*>(expr)->name, defineParent)));
        return;
    }
    case SAT_IfElse: {
        GenIfElseByteCode(dynamic_cast<IfElseExprAST*>(expr), defineParent);
        return;
    }
    case AT_Func : {
        GenFuncByteCode(dynamic_cast<CallExprAST*>(expr), defineParent);
        return;
    }
    case SAT_Define: {
        GenDefine(expr, defineParent);
        return;
    }
    case SAT_Lambda: {
        GenLambdaByteCode(dynamic_cast<LambdaExprAST*>(expr), defineParent);
        return;
    }
    default: {
        command.push_back(std::shared_ptr<TByteCodeCMD>(new TByteCodeCMDPush(GetAllocatorValue(expr))));
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
