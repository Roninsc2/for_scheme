#include "byte_code_cmd.h"
#include <algorithm>
#include <iterator>
#include "scheme_std_lib.cpp"


const std::map<std::string, ExprType*(*)(std::vector< std::shared_ptr<ExprType> >&) >TByteCodeCMD::stdFuncMap  = { (std::make_pair("+", &plus)),  (std::make_pair("-", &minus)),
                              (std::make_pair("/", &division)), (std::make_pair("*", &mult)), (std::make_pair("cons", &cons)), (std::make_pair("append", &append)),
                              (std::make_pair("list", &GetList)), (std::make_pair("=", &equally)), (std::make_pair("define", &defineFun)) };


TByteCodeCMDPush::TByteCodeCMDPush(size_t valNum){
    valueNumber = valNum;
}

void TByteCodeCMDPush::UpdateStack(TStack& Stack)
{
    Stack.StackPushBack(Stack.GetAllocatorAt(valueNumber));
}


TByteCodeCMDPushIdent::TByteCodeCMDPushIdent(std::string val) {
    value = val;
}

void TByteCodeCMDPushIdent::UpdateStack(TStack& Stack)
{
    Enviroment* env = Stack.GetCurrentEnviroment()->FindVarible(value);
    if (env) {
        Stack.StackPushBack(dynamic_cast<IdentType*>(env->Find(value))->GetValue());
    }
}


TByteCodeCMDIfElse::TByteCodeCMDIfElse(size_t& IT)
    :it(IT)
{
}

void TByteCodeCMDIfElse::UpdateStack(TStack& Stack)
{
    std::shared_ptr<ExprType> expr = Stack.StackPop();
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
    define->ResetParent(&defParent);
}

void TByteCodeCMDDefine::UpdateStack(TStack& Stack)
{
    PrototypeType* Proto = new PrototypeType(Args);
    size_t start = it;
    size_t count = 0;
    FunctionType* func = new FunctionType(Proto, start, define);
    while (it < command.size()) {
        it++;
        if (!count && dynamic_cast<TByteCodeCMDJumpEndDefine*>(command[it].get())) {
            func->ResetEnd(it);
            define->GetParent()->InsertToEnviroment(funcName , std::shared_ptr<ExprType>(func));
            break;
        } else if (dynamic_cast<TByteCodeCMDDefine*>(command[it].get())) {
            count++;
        } else if (dynamic_cast<TByteCodeCMDJumpEndDefine*>(command[it].get())) {
            count--;
        }
    }
}

Enviroment *TByteCodeCMDDefine::GetEnviroment() {
    return define.get();
}


TByteCodeCMDCall::TByteCodeCMDCall(std::string callee, size_t i, size_t& IT, std::vector<std::shared_ptr<TByteCodeCMD>>& Command)
    :size(i), it(IT), command(Command)
{
    name = callee;
}

void TByteCodeCMDCall::UpdateStack(TStack& Stack)
{
    if (size > Stack.GetStackSize()) {
        return;
    }
    std::vector<std::shared_ptr<ExprType>> exprs;
    for (size_t i = 0; i < size; i++) {
        exprs.push_back(Stack.StackPop());
    }
    if (stdFuncMap.count(name)) {
        Stack.StackPushBack(std::shared_ptr<ExprType>(stdFuncMap.at(name)(exprs)));
        return;
    }
    Enviroment* env = Stack.GetCurrentEnviroment()->FindVarible(name);
    if (env) {
        FunctionType* Function = dynamic_cast<FunctionType*>(env->Find(name));
        Stack.ResetCurrentEnviroment(Function->GetEnviroment());
        if (exprs.size() == Function->GetPrototype()->GetArgsSize()) {
            size_t j = exprs.size()-1;
            for (size_t i = 0; i <  Function->GetPrototype()->GetArgsSize(); i++) {
                dynamic_cast<IdentType*>(Stack.GetCurrentEnviroment()->Find(Function->GetPrototype()->GetArgsAt(i)))->GetValue()= exprs.at(j);
                j--;
            }
            size_t current_position = it;
            it = Function->GetStart();
            TByteCodeCMDJumpEndDefine* endDefine = dynamic_cast<TByteCodeCMDJumpEndDefine*>(command[Function->GetEnd()].get());
            endDefine->SetCurrentPos(current_position);
        }
    }
}


TByteCodeCMDTailCall::TByteCodeCMDTailCall(std::string callee, size_t p, size_t i, size_t& IT)
    :size(i), it(IT)
{
    name = callee;
    pos = p-1;
}

void TByteCodeCMDTailCall::UpdateStack(TStack& Stack)
{
    if (size > Stack.GetStackSize()) {
        return;
    }
    std::vector<std::shared_ptr<ExprType>> exprs;
    for (size_t i = 0; i < size; i++) {
        exprs.push_back(Stack.StackPop());
    }
    Enviroment* env = Stack.GetCurrentEnviroment()->FindVarible(name);
    if (env) {
        FunctionType* Function = dynamic_cast<FunctionType*>(env->Find(name));
        Stack.ResetCurrentEnviroment(Function->GetEnviroment());
        if (exprs.size() == Function->GetPrototype()->GetArgsSize()) {
            size_t j = exprs.size()-1;
            for (size_t i = 0; i <  Function->GetPrototype()->GetArgsSize(); i++) {
                dynamic_cast<IdentType*>(Stack.GetCurrentEnviroment()->Find(Function->GetPrototype()->GetArgsAt(i)))->ResetValue(exprs.at(j));
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
    Stack.AllocatorPushBack(std::shared_ptr<ExprType>(new NumberIntType(value)));
}


TByteCodeCMDAllocDouble::TByteCodeCMDAllocDouble(double val)
{
    value = val;
}

void TByteCodeCMDAllocDouble::UpdateStack(TStack& Stack)
{
    Stack.AllocatorPushBack(std::shared_ptr<ExprType>(new NumberDoubleType(value)));
}

TByteCodeCMDAllocChar::TByteCodeCMDAllocChar(char val)
{
    value = val;
}

void TByteCodeCMDAllocChar::UpdateStack(TStack& Stack)
{
    Stack.AllocatorPushBack(std::shared_ptr<ExprType>(new CharType(value)));
}

TByteCodeCMDAllocString::TByteCodeCMDAllocString(std::string val)
{
    value = val;
}

void TByteCodeCMDAllocString::UpdateStack(TStack& Stack)
{
    Stack.AllocatorPushBack(std::shared_ptr<ExprType>(new StringType(value)));
}

TByteCodeCMDAllocSymbol::TByteCodeCMDAllocSymbol(std::string val)
{
    value = val;
}

void TByteCodeCMDAllocSymbol::UpdateStack(TStack& Stack)
{
    Stack.AllocatorPushBack(std::shared_ptr<ExprType>(new SymbolType(value)));
}

TByteCodeCMDAllocBool::TByteCodeCMDAllocBool(bool val)
{
    value = val;
}

void TByteCodeCMDAllocBool::UpdateStack(TStack& Stack)
{
    Stack.AllocatorPushBack(std::shared_ptr<ExprType>(new BoolType(value)));
}

TByteCodeCMDLambda::TByteCodeCMDLambda(std::vector<std::string> idents, Enviroment& defParent)
    : args(idents)
{
    define->ResetParent(&defParent);
    for (size_t i = 0; i < idents.size(); i++) {
        define->InsertToEnviroment(idents[i], std::shared_ptr<IdentType>(new IdentType(idents[i])));
    }
}

void TByteCodeCMDLambda::UpdateStack(TStack &Stack)
{
    size_t size = args.size();
    if (size > Stack.GetStackSize()) {
        return;
    }
    std::vector<std::shared_ptr<ExprType>> exprs;
    for (size_t i = 0; i < size; i++) {
        exprs.push_back(Stack.StackPop());
    }
    Stack.ResetCurrentEnviroment(define.get());
    size_t j = exprs.size()-1;
    for (size_t i = 0; i < args.size(); i++) {
        dynamic_cast<IdentType*>(Stack.GetCurrentEnviroment()->Find(args[i]))->ResetValue(exprs.at(j));
        j--;
    }
}

void TByteCodeCMDJump::SetCurrentPos(size_t pos) {
    currentPos = pos;
}

void TByteCodeCMDJumpEndDefine::SetCurrentPos(size_t pos) {
    currentPos = pos;
}
