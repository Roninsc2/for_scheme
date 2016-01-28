#include "byte_code_cmd.h"
#include <algorithm>
#include <iterator>
#include "scheme_std_lib.cpp"

std::shared_ptr<ExprType> StackPop(TStack& Stack);

TByteCodeCMDPush::TByteCodeCMDPush(size_t valNum, TStack& stack): Stack(stack) {
    Type = ECMD_PUSH;
    valueNumber = valNum;
}

void TByteCodeCMDPush::UpdateStack()
{
    Stack.stack.push_back(Stack.allocator.at(valueNumber));
}


TByteCodeCMDPushIdent::TByteCodeCMDPushIdent(std::string val, TStack& stack): Stack(stack) {
    Type = ECMD_PUSHIDENT;
    value = val;
}

void TByteCodeCMDPushIdent::UpdateStack()
{
    Stack.stack.push_back(Stack.defineVar.at(value)->value);
}


TByteCodeCMDIfElse::TByteCodeCMDIfElse(TStack& stack, std::vector<std::shared_ptr<TByteCodeCMD>>& cmd, size_t& IT)
    : Stack(stack), command(cmd), it(IT)
{
    Type = ECMD_IFELSE;
}

void TByteCodeCMDIfElse::UpdateStack()
{
    it++;
    std::shared_ptr<ExprType> expr;
    if (command[it]->Type == ECMD_TAILCALL) {
        command[it]->UpdateStack();
        return;
    } else {
        command[it]->UpdateStack();
        expr = StackPop(Stack);
    }
    it++;
    if (expr->Type == T_None || (expr->Type == T_Bool && !(dynamic_cast<BoolType*>(expr.get())->value))) {
        Skip(command, it);
        if (command.at(it)->Type == ECMD_ENDCALL) {
            Stack.stack.push_back(std::shared_ptr<ExprType>(new NoneType()));
            return;
        } else {
            if (command[it]->Type == ECMD_TAILCALL) {
                command[it]->UpdateStack();
                return;
            } else {
                command[it]->UpdateStack();
                expr = StackPop(Stack);
            }
        }
    } else {
        if (command[it]->Type == ECMD_TAILCALL) {
            command[it]->UpdateStack();
            return;
        } else {
            command[it]->UpdateStack();
            expr = StackPop(Stack);
        }
    }
    Stack.stack.push_back(expr);
    it++;
    if (command.at(it)->Type == ECMD_ENDCALL) {
        return;
    } else {
        Skip(command, it);
    }
}

void TByteCodeCMDIfElse::Skip(std::vector<std::shared_ptr<TByteCodeCMD>> command, size_t &it)
{
    if (command.at(it)->Type == ECMD_PUSH || command.at(it)->Type == ECMD_PUSHIDENT) {
        it++;
        return;
    } else {
        it++;
        while (it < command.size() && command.at(it)->Type != ECMD_ENDCALL) {
            if (command.at(it)->Type != ECMD_PUSH || command.at(it)->Type != ECMD_PUSHIDENT) {
                Skip(command, it);
            }
            it++;
        }
        it++;
    }
}


TByteCodeCMDDefine::TByteCodeCMDDefine(std::string name, size_t size, std::vector<std::string> idents, TStack& stack, std::vector<std::shared_ptr<TByteCodeCMD>>& cmd, size_t& IT)
    : Stack(stack), command(cmd), it(IT)
{

    Type = ECMD_DEFSTART;
    funcName = name;
    for (size_t i = 0; i < size; i++) {
        args.insert(std::make_pair(idents[i], std::shared_ptr<IdentType>(new IdentType(idents[i]))));
    }
}

void TByteCodeCMDDefine::UpdateStack()
{
    PrototypeType* proto = new PrototypeType(funcName, args);
    size_t start = it+1;
    while (it < command.size()) {
        it++;
        if ((ECommand)command[it]->Type == ECMD_ENDDEF) {
            size_t end = it;
            Stack.defineFunc.insert(std::make_pair(funcName ,std::shared_ptr<FunctionType>(new FunctionType(proto, start, end))));
            break;
        }
    }
}


TByteCodeCMDCall::TByteCodeCMDCall(std::string callee, TStack& stack, std::vector<std::shared_ptr<TByteCodeCMD>>& cmd, size_t& IT)
    : Stack(stack), command(cmd), it(IT)
{
    Type = ECMD_CALL;
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

void TByteCodeCMDCall::UpdateStack()
{
    std::vector<std::shared_ptr<ExprType>> exprs;
    while (it < command.size()) {
        it++;
        if (command[it]->Type == ECMD_ENDCALL) {
            break;
        }
        command[it]->UpdateStack();
        exprs.push_back(StackPop(Stack));
    }
    if (name == "define") {
        IdentType* ident = (IdentType*)stdFuncMap.at(name)(exprs);
        Stack.defineVar.insert(std::make_pair(ident->name, std::shared_ptr<IdentType>(ident)));
    }
    if (stdFuncMap.count(name)) {
        Stack.stack.push_back(std::shared_ptr<ExprType>(stdFuncMap.at(name)(exprs)));
    } else if (Stack.defineFunc.count(name)) {
        if (exprs.size() == Stack.defineFunc.at(name)->Proto->Args.size()) {
            size_t j = 0;
            std::shared_ptr<ExprType> result;
            std::map<std::string, std::shared_ptr<IdentType> > defineVarBuffer = Stack.defineVar;
            std::map<std::string, std::shared_ptr<FunctionType> > defineFuncBuffer = Stack.defineFunc;
            for (auto i = Stack.defineFunc.at(name)->Proto->Args.begin(); i != Stack.defineFunc.at(name)->Proto->Args.end(); i++) {
                i->second->value.reset();
                i->second->value = exprs.at(j);
                if (Stack.defineVar.count(i->first)) {
                    Stack.defineVar.erase(i->first);
                }
                Stack.defineVar.insert(std::make_pair(i->first, i->second));
                j++;
            }
            size_t current_position = it;
            it = Stack.defineFunc.at(name)->Start;
            while (it < Stack.defineFunc.at(name)->End) {
                if (command[it]->Type == ECMD_TAILCALL) {
                    command[it]->UpdateStack();
                } else {
                    command[it]->UpdateStack();
                    result = StackPop(Stack);
                }
                it++;
            }
            it = current_position;
            Stack.defineVar = defineVarBuffer;
            Stack.defineFunc = defineFuncBuffer;
            Stack.stack.push_back(result);
        }

    }
}


TByteCodeCMDTailCall::TByteCodeCMDTailCall(std::string callee, size_t p, TStack& stack, std::vector<std::shared_ptr<TByteCodeCMD>>& cmd, size_t& IT)
    : Stack(stack), command(cmd), it(IT)
{
    Type = ECMD_TAILCALL;
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

void TByteCodeCMDTailCall::UpdateStack()
{
    std::vector<std::shared_ptr<ExprType>> exprs;
    while (it < command.size()) {
        it++;
        if (command[it]->Type == ECMD_ENDCALL) {
            break;
        }
        command[it]->UpdateStack();
        exprs.push_back(StackPop(Stack));
    }
    if (Stack.defineFunc.count(name)) {
        if (exprs.size() == Stack.defineFunc.at(name)->Proto->Args.size()) {
            size_t j = 0;
            for (auto i = Stack.defineFunc.at(name)->Proto->Args.begin(); i != Stack.defineFunc.at(name)->Proto->Args.end(); i++) {
                if (Stack.defineVar.count(i->first)) {
                    Stack.defineVar.at(i->first)->value.reset();
                    Stack.defineVar.at(i->first)->value = exprs.at(j);
                }
                j++;
            }
            it = pos-1;
        }
    }
}


TByteCodeCMDAllocInt::TByteCodeCMDAllocInt(int val, TStack& stack) : Stack(stack)
{
    Type = ECMD_AllOCINT;
    value = val;
}

void TByteCodeCMDAllocInt::UpdateStack()
{
    Stack.allocator.push_back(std::shared_ptr<ExprType>(new NumberIntType(value)));
}


TByteCodeCMDAllocDouble::TByteCodeCMDAllocDouble(double val, TStack& stack): Stack(stack)
{
    Type = ECMD_AllOCDOUBLE;
    value = val;
}

void TByteCodeCMDAllocDouble::UpdateStack()
{
    Stack.allocator.push_back(std::shared_ptr<ExprType>(new NumberDoubleType(value)));
}

TByteCodeCMDAllocChar::TByteCodeCMDAllocChar(char val, TStack& stack): Stack(stack)
{
    Type = ECMD_AllOCCHAR;
    value = val;
}

void TByteCodeCMDAllocChar::UpdateStack()
{
    Stack.allocator.push_back(std::shared_ptr<ExprType>(new CharType(value)));
}

TByteCodeCMDAllocString::TByteCodeCMDAllocString(std::string val, TStack& stack): Stack(stack)
{
    Type = ECMD_AllOCSTRING;
    value = val;
}

void TByteCodeCMDAllocString::UpdateStack()
{
    Stack.allocator.push_back(std::shared_ptr<ExprType>(new StringType(value)));
}

TByteCodeCMDAllocSymbol::TByteCodeCMDAllocSymbol(std::string val, TStack& stack): Stack(stack)
{
    Type = ECMD_AllOCSYMBOL;
    value = val;
}

void TByteCodeCMDAllocSymbol::UpdateStack()
{
    Stack.allocator.push_back(std::shared_ptr<ExprType>(new SymbolType(value)));
}

TByteCodeCMDAllocBool::TByteCodeCMDAllocBool(bool val, TStack& stack): Stack(stack)
{
    Type = ECMD_AllOCBOOL;
    value = val;
}

void TByteCodeCMDAllocBool::UpdateStack()
{
    Stack.allocator.push_back(std::shared_ptr<ExprType>(new BoolType(value)));
}

std::shared_ptr<ExprType> StackPop(TStack& Stack) {
    if (Stack.stack.size()) {
        std::shared_ptr<ExprType> expr = Stack.stack.at(Stack.stack.size()-1);
        Stack.stack.pop_back();
        return expr;
    } else {
        return std::shared_ptr<ExprType>(new NoneType());
    }
}
