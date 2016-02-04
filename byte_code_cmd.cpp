#include "byte_code_cmd.h"
#include <algorithm>
#include <iterator>
#include "scheme_std_lib.cpp"

std::shared_ptr<ExprType> StackPop(TStack& Stack);

TByteCodeCMDPush::TByteCodeCMDPush(size_t valNum){
    valueNumber = valNum;
}

void TByteCodeCMDPush::UpdateStack(TStack& Stack)
{
    Stack.stack.push_back(Stack.allocator.at(valueNumber));
}


TByteCodeCMDPushIdent::TByteCodeCMDPushIdent(std::string val){
    value = val;
}

void TByteCodeCMDPushIdent::UpdateStack(TStack& Stack)
{
    Stack.stack.push_back(Stack.defineVar.at(value)->value);
}


TByteCodeCMDIfElse::TByteCodeCMDIfElse(std::vector<std::shared_ptr<TByteCodeCMD>>& cmd, size_t& IT)
    :command(cmd), it(IT)
{
}

void TByteCodeCMDIfElse::UpdateStack(TStack& Stack)
{
    it++;
    std::shared_ptr<ExprType> expr;
    while (it < command.size()) {
        if (dynamic_cast<TByteCodeCMDEndCall*>(command[it].get())) {
            break;
        } else if (dynamic_cast<TByteCodeCMDTailCall*>(command[it].get())) {
            command[it]->UpdateStack(Stack);
            return;
        } else {
            command[it]->UpdateStack(Stack);
        }
        it++;
    }
    expr = StackPop(Stack);
    it++;
    if (dynamic_cast<NoneType*>(expr.get()) || (dynamic_cast<BoolType*>(expr.get()) && !(dynamic_cast<BoolType*>(expr.get())->value))) {
        Skip(command, it);
        it++;
        if (dynamic_cast<TByteCodeCMDEndCall*>(command.at(it).get())) {
            Stack.stack.push_back(std::shared_ptr<ExprType>(new NoneType()));
            return;
        } else {
            while (it < command.size()) {
                if (dynamic_cast<TByteCodeCMDEndCall*>(command[it].get())) {
                    break;
                } else if (dynamic_cast<TByteCodeCMDTailCall*>(command[it].get())) {
                    command[it]->UpdateStack(Stack);
                    return;
                } else {
                    command[it]->UpdateStack(Stack);
                }
                it++;
            }
            return;
        }
    } else {
        while (it < command.size()) {
            if (dynamic_cast<TByteCodeCMDEndCall*>(command[it].get())) {
                break;
            } else if (dynamic_cast<TByteCodeCMDTailCall*>(command[it].get())) {
                command[it]->UpdateStack(Stack);
                return;
            } else {
                command[it]->UpdateStack(Stack);
            }
            it++;
        }
    }
    it++;
    if (dynamic_cast<TByteCodeCMDEndCall*>(command.at(it).get())) {
        return;
    } else {
        Skip(command, it);
    }
}

void TByteCodeCMDIfElse::Skip(std::vector<std::shared_ptr<TByteCodeCMD>> command, size_t &it)
{
    while (!dynamic_cast<TByteCodeCMDEndCall*>(command[it].get())) {
        it++;
    }
}




TByteCodeCMDDefine::TByteCodeCMDDefine(std::string name, size_t size, std::vector<std::string> idents, std::vector<std::shared_ptr<TByteCodeCMD>>& cmd, size_t& IT)
    :command(cmd), it(IT)
{
    funcName = name;
    for (size_t i = 0; i < size; i++) {
        args.insert(std::make_pair(idents[i], std::shared_ptr<IdentType>(new IdentType(idents[i]))));
    }
}

void TByteCodeCMDDefine::UpdateStack(TStack& Stack)
{
    PrototypeType* proto = new PrototypeType(funcName, args);
    size_t start = it;
    size_t count = 0;
    while (it < command.size()) {
        it++;
        if (count && dynamic_cast<TByteCodeCMDEndDef*>(command[it].get())) {
            count--;
        } else if (!count && dynamic_cast<TByteCodeCMDEndDef*>(command[it].get())) {
            size_t end = it;
            Stack.defineFunc.insert(std::make_pair(funcName ,std::shared_ptr<FunctionType>(new FunctionType(proto, start, end))));
            break;
        } else if (dynamic_cast<TByteCodeCMDDefine*>(command[it].get())) {
            count++;
        }
    }
}


TByteCodeCMDCall::TByteCodeCMDCall(std::string callee, size_t i, size_t& IT, std::vector<std::shared_ptr<TByteCodeCMD>>& Command)
    :size(i), it(IT), command(Command)
{
    name = callee;
    stdFuncMap.insert(std::make_pair("+", &plus));
    stdFuncMap.insert(std::make_pair("-", &minus));
    stdFuncMap.insert(std::make_pair("/", &division));
    stdFuncMap.insert(std::make_pair("*", &mult));
    stdFuncMap.insert(std::make_pair("cons", &cons));
    stdFuncMap.insert(std::make_pair("append", &append));
    stdFuncMap.insert(std::make_pair("list", &list));
    stdFuncMap.insert(std::make_pair("=", &equally));
    stdFuncMap.insert(std::make_pair("define", &defineFun));
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
    if (name == "define") {
        IdentType* ident = (IdentType*)stdFuncMap.at(name)(exprs);
        Stack.defineVar.insert(std::make_pair(ident->name, std::shared_ptr<IdentType>(ident)));
    } else if (stdFuncMap.count(name)) {
        Stack.stack.push_back(std::shared_ptr<ExprType>(stdFuncMap.at(name)(exprs)));
    } else if (Stack.defineFunc.count(name)) {
        if (exprs.size() == Stack.defineFunc.at(name)->Proto->Args.size()) {
            size_t j = exprs.size()-1;
            std::map<std::string, std::shared_ptr<IdentType> > defineVarBuffer = Stack.defineVar;
            std::map<std::string, std::shared_ptr<FunctionType> > defineFuncBuffer = Stack.defineFunc;
            for (auto i = Stack.defineFunc.at(name)->Proto->Args.begin(); i != Stack.defineFunc.at(name)->Proto->Args.end(); i++) {
                i->second->value.reset();
                i->second->value = exprs.at(j);
                if (Stack.defineVar.count(i->first)) {
                    Stack.defineVar.erase(i->first);
                }
                Stack.defineVar.insert(std::make_pair(i->first, i->second));
                j--;
            }
            size_t current_position = it;
            it = Stack.defineFunc.at(name)->Start;
            TByteCodeCMDEndDef* endDefine = dynamic_cast<TByteCodeCMDEndDef*>(command[Stack.defineFunc.at(name)->End].get());
            endDefine->currentPos.push_back(current_position);
            endDefine->defineFunctionBuffer = defineFuncBuffer;
            endDefine->defineVaribleBuffer = defineVarBuffer;
        }
    }
}


TByteCodeCMDTailCall::TByteCodeCMDTailCall(std::string callee, size_t p, size_t i, size_t& IT)
    :size(i), it(IT)
{
    name = callee;
    pos = p;
    stdFuncMap.insert(std::make_pair("+", &plus));
    stdFuncMap.insert(std::make_pair("-", &minus));
    stdFuncMap.insert(std::make_pair("/", &division));
    stdFuncMap.insert(std::make_pair("*", &mult));
    stdFuncMap.insert(std::make_pair("cons", &cons));
    stdFuncMap.insert(std::make_pair("append", &append));
    stdFuncMap.insert(std::make_pair("list", &list));
    stdFuncMap.insert(std::make_pair("=", &equally));
    stdFuncMap.insert(std::make_pair("define", &defineFun));
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
    if (Stack.defineFunc.count(name)) {
        if (exprs.size() == Stack.defineFunc.at(name)->Proto->Args.size()) {
            size_t j = exprs.size()-1;
            for (auto i = Stack.defineFunc.at(name)->Proto->Args.begin(); i != Stack.defineFunc.at(name)->Proto->Args.end(); i++) {
                if (Stack.defineVar.count(i->first)) {
                    Stack.defineVar.at(i->first)->value.reset();
                    Stack.defineVar.at(i->first)->value = exprs.at(j);
                }
                j--;
            }
            it = pos-1;
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
    size_t size = args.size();
    if (size > Stack.stack.size()) {
        return;
    }
    std::vector<std::shared_ptr<ExprType>> exprs;
    for (size_t i = 0; i < size; i++) {
        exprs.push_back(StackPop(Stack));
    }
    size_t j = exprs.size()-1;
    std::map<std::string, std::shared_ptr<IdentType> > defineVarBuffer = Stack.defineVar;
    std::map<std::string, std::shared_ptr<FunctionType> > defineFuncBuffer = Stack.defineFunc;
    for (auto i = args.begin(); i != args.end(); i++) {
        i->second->value.reset();
        i->second->value = exprs.at(j);
        if (Stack.defineVar.count(i->first)) {
            Stack.defineVar.erase(i->first);
        }
        Stack.defineVar.insert(std::make_pair(i->first, i->second));
        j--;
    }
    TByteCodeCMDEndLambda* endLambda = dynamic_cast<TByteCodeCMDEndLambda*>(command[end].get());
    endLambda->defineFunctionBuffer = defineFuncBuffer;
    endLambda->defineVaribleBuffer = defineVarBuffer;
}
