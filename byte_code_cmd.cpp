#include "byte_code_cmd.h"
#include <algorithm>
#include <iterator>
#include "scheme_std_lib.cpp"


std::map<std::string, ExprType*(*)(std::vector< std::shared_ptr<ExprType> >&) > stdFuncMap = { (std::make_pair("+", &plus)),  (std::make_pair("-", &minus)),
        (std::make_pair("/", &division)), (std::make_pair("*", &mult)), (std::make_pair("cons", &cons)), (std::make_pair("append", &append)),
        (std::make_pair("list", &list)), (std::make_pair("=", &equally)), (std::make_pair("define", &defineFun)) };

Enviroment* FindVarible(std::string name, Enviroment* env);

std::shared_ptr<ExprType> StackPop(TStack& Stack);

TByteCodeCMDPush::TByteCodeCMDPush(size_t valNum){
    valueNumber = valNum;
}

void TByteCodeCMDPush::UpdateStack(TStack& Stack)
{
    Stack.stack.push_back(Stack.allocator.at(valueNumber));
}


TByteCodeCMDPushIdent::TByteCodeCMDPushIdent(std::string val, Enviroment& defParent) : defineParent(defParent){
    value = val;
}

void TByteCodeCMDPushIdent::UpdateStack(TStack& Stack)
{
    Enviroment* env = FindVarible(value, &defineParent);
    if (env) {
        Stack.stack.push_back(dynamic_cast<IdentType*>(env->This.at(value).get())->value);
    }
}


TByteCodeCMDIfElse::TByteCodeCMDIfElse(size_t& IT)
    :it(IT)
{
}

void TByteCodeCMDIfElse::UpdateStack(TStack& Stack)
{
    std::shared_ptr<ExprType> expr = StackPop(Stack);
    if (expr->IsTrue()) {
        it++;
    }
}



TByteCodeCMDDefine::TByteCodeCMDDefine(std::string name, std::vector<std::string> idents,
                                       Enviroment& defParent,
                                       std::vector<std::shared_ptr<TByteCodeCMD>>& cmd, size_t& IT): command(cmd), it(IT)
{
    funcName = name;
    Args = idents;
    define->Parent = &defParent;
}

void TByteCodeCMDDefine::UpdateStack(TStack& Stack)
{
    PrototypeType* proto = new PrototypeType(funcName, Args);
    size_t start = it;
    size_t count = 0;
    FunctionType* func = new FunctionType(proto, start, define);
    while (it < command.size()) {
        it++;
        if (!count && dynamic_cast<TByteCodeCMDJumpEndDefine*>(command[it].get())) {
            func->End = it;
            define->Parent->This.insert(std::make_pair(funcName , std::shared_ptr<ExprType>(func)));
            break;
        } else if (dynamic_cast<TByteCodeCMDDefine*>(command[it].get())) {
            count++;
        } else if (dynamic_cast<TByteCodeCMDJumpEndDefine*>(command[it].get())) {
            count--;
        }
    }
}


TByteCodeCMDCall::TByteCodeCMDCall(std::string callee, size_t i, size_t& IT, std::vector<std::shared_ptr<TByteCodeCMD>>& Command, Enviroment& defParent)
    :size(i), it(IT), command(Command), defineParent(defParent)
{
    name = callee;
}

void TByteCodeCMDCall::UpdateStack(TStack& Stack)
{
    if (size > Stack.stack.size()) {
        return;
    }
    std::vector<std::shared_ptr<ExprType>> exprs;
    for (size_t i = 0; i < size; i++) {
        exprs.push_back(StackPop(Stack));
    }
    if (stdFuncMap.count(name)) {
        Stack.stack.push_back(std::shared_ptr<ExprType>(stdFuncMap.at(name)(exprs)));
        return;
    }
    Enviroment* env = FindVarible(name, &defineParent);
    if (env) {
        FunctionType* Function = dynamic_cast<FunctionType*>(env->This.at(name).get());
        if (exprs.size() == Function->Proto->Args.size()) {
            size_t j = exprs.size()-1;
            for (size_t i = 0; i <  Function->Proto->Args.size(); i++) {
                dynamic_cast<IdentType*>(Function->Env->This.at(Function->Proto->Args[i]).get())->value.reset();
                dynamic_cast<IdentType*>(Function->Env->This.at(Function->Proto->Args[i]).get())->value = exprs.at(j);
                j--;
            }
            size_t current_position = it;
            it = Function->Start;
            TByteCodeCMDJumpEndDefine* endDefine = dynamic_cast<TByteCodeCMDJumpEndDefine*>(command[Function->End].get());
            endDefine->currentPos = current_position;
        }
    }
}


TByteCodeCMDTailCall::TByteCodeCMDTailCall(std::string callee, size_t p, size_t i, size_t& IT, Enviroment& defParent)
    :size(i), it(IT), defineParent(defParent)
{
    name = callee;
    pos = p-1;
}

void TByteCodeCMDTailCall::UpdateStack(TStack& Stack)
{
    if (size > Stack.stack.size()) {
        return;
    }
    std::vector<std::shared_ptr<ExprType>> exprs;
    for (size_t i = 0; i < size; i++) {
        exprs.push_back(StackPop(Stack));
    }
    Enviroment* env = FindVarible(name, &defineParent);
    if (env) {
        FunctionType* Function = dynamic_cast<FunctionType*>(env->This.at(name).get());
        if (exprs.size() == Function->Proto->Args.size()) {
            size_t j = exprs.size()-1;
            for (size_t i = 0; i <  Function->Proto->Args.size(); i++) {
                dynamic_cast<IdentType*>(Function->Env->This.at(Function->Proto->Args[i]).get())->value.reset();
                dynamic_cast<IdentType*>(Function->Env->This.at(Function->Proto->Args[i]).get())->value = exprs.at(j);
                j--;
            }
            it = pos;
        }
    }
}


TByteCodeCMDAllocInt::TByteCodeCMDAllocInt(int val)
{
    value = val;
}

void TByteCodeCMDAllocInt::UpdateStack(TStack& Stack)
{
    Stack.allocator.push_back(std::shared_ptr<ExprType>(new NumberIntType(value)));
}


TByteCodeCMDAllocDouble::TByteCodeCMDAllocDouble(double val)
{
    value = val;
}

void TByteCodeCMDAllocDouble::UpdateStack(TStack& Stack)
{
    Stack.allocator.push_back(std::shared_ptr<ExprType>(new NumberDoubleType(value)));
}

TByteCodeCMDAllocChar::TByteCodeCMDAllocChar(char val)
{
    value = val;
}

void TByteCodeCMDAllocChar::UpdateStack(TStack& Stack)
{
    Stack.allocator.push_back(std::shared_ptr<ExprType>(new CharType(value)));
}

TByteCodeCMDAllocString::TByteCodeCMDAllocString(std::string val)
{
    value = val;
}

void TByteCodeCMDAllocString::UpdateStack(TStack& Stack)
{
    Stack.allocator.push_back(std::shared_ptr<ExprType>(new StringType(value)));
}

TByteCodeCMDAllocSymbol::TByteCodeCMDAllocSymbol(std::string val)
{
    value = val;
}

void TByteCodeCMDAllocSymbol::UpdateStack(TStack& Stack)
{
    Stack.allocator.push_back(std::shared_ptr<ExprType>(new SymbolType(value)));
}

TByteCodeCMDAllocBool::TByteCodeCMDAllocBool(bool val)
{
    value = val;
}

void TByteCodeCMDAllocBool::UpdateStack(TStack& Stack)
{
    Stack.allocator.push_back(std::shared_ptr<ExprType>(new BoolType(value)));
}

std::shared_ptr<ExprType> StackPop(TStack& Stack) {
    std::shared_ptr<ExprType> expr = Stack.stack.at(Stack.stack.size()-1);
    Stack.stack.pop_back();
    return expr;
}

TByteCodeCMDLambda::TByteCodeCMDLambda(std::vector<std::string> idents, std::vector<std::shared_ptr<TByteCodeCMD>>& cmd): command(cmd)
{
    for (size_t i = 0; i < idents.size(); i++) {
        args.insert(std::make_pair(idents[i], std::shared_ptr<IdentType>(new IdentType(idents[i]))));
    }
}

void TByteCodeCMDLambda::UpdateStack(TStack &Stack)
{
    /*size_t size = args.size();
    if (size > Stack.stack.size()) {
        return;
    }
    std::vector<std::shared_ptr<ExprType>> exprs;
    for (size_t i = 0; i < size; i++) {
        exprs.push_back(StackPop(Stack));
    }
    size_t j = exprs.size()-1;
    for (auto i = args.begin(); i != args.end(); i++) {
        i->second->value.reset();
        i->second->value = exprs.at(j);
        if (Stack.define.count(i->first)) {
            Stack.define.erase(i->first);
        }
        Stack.define.insert(std::make_pair(i->first, i->second));
        j--;
    }*/
}

Enviroment* FindVarible(std::string name, Enviroment* env)
{
    while (env->Parent) {
        if (env->This.count(name)) {
            return env;
        }
        env = env->Parent;
    }
    if (env->This.count(name)) {
        return env;
    }
    return nullptr;
}
