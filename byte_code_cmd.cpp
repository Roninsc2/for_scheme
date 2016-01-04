#include "byte_code_cmd.h"
#include <algorithm>
#include <iterator>
#include "scheme_std_lib.cpp"

TByteCodeCMDPush::TByteCodeCMDPush(size_t valNum) {
    Type = ECMD_PUSH;
    valueNumber = valNum;
}

void TByteCodeCMDPush::UpdateStack(std::vector<ExprType*>& stack, std::vector<ExprType*> allocator)
{
    stack.push_back(allocator.at(valueNumber));
}


TByteCodeCMDPushIdent::TByteCodeCMDPushIdent(std::string val) {
    Type = ECMD_PUSHIDENT;
    value = val;
}

void TByteCodeCMDPushIdent::UpdateStack(std::vector<ExprType*>& stack, std::map<std::string, IdentType* > defineVar)
{
    stack.push_back(defineVar.at(value)->value);
}


TByteCodeCMDIfElse::TByteCodeCMDIfElse(std::vector<ExprType *> *stack, std::stringstream &str): byteCodeString(str) {
    Stack = stack;
    Type = ECMD_IFELSE;
}

void TByteCodeCMDIfElse::UpdateStack()
{

}


TByteCodeCMDDefine::TByteCodeCMDDefine(std::string name, size_t size, std::vector<std::string> idents) {

    Type = ECMD_DEFSTART;
    funcName = name;
    size_t sizeArgs = size;
    for (size_t i = 0; i < sizeArgs; i++) {
        args.insert(std::make_pair(idents[i], new IdentType(idents[i])));
    }
}

void TByteCodeCMDDefine::UpdateStack(std::map<std::string, FunctionType* >& Define, std::vector<TByteCodeCMD*> command, size_t& it)
{
    PrototypeType* proto = new PrototypeType(funcName, args);
    size_t start = it;
    while (it < command.size()) {
        it++;
        if ((ECommand)command[it]->Type == ECMD_ENDDEF) {
            size_t end = it;
            Define.insert(std::make_pair(funcName ,new FunctionType(proto, start, end)));
            break;
        }
    }
}


TByteCodeCMDCall::TByteCodeCMDCall(std::string callee)
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

void TByteCodeCMDCall::UpdateStack(std::vector<ExprType*>& Stack, std::vector<ExprType*> allocator, std::map<std::string, IdentType* > defineVar,
                                   std::map<std::string, FunctionType* > Define, std::vector<TByteCodeCMD*> command, size_t& it)
{
    std::vector<ExprType*> exprs;
    while (it < command.size()) {
        it++;
        if (command[it]->Type == ECMD_ENDCALL) {
            break;
        }
        switch (command[it]->Type) {
        case ECMD_PUSH: {
            dynamic_cast<TByteCodeCMDPush*>(command[it])->UpdateStack(exprs, allocator);
            break;
        }
        case ECMD_PUSHIDENT: {
            dynamic_cast<TByteCodeCMDPushIdent*>(command[it])->UpdateStack(exprs, defineVar);
            break;
        }
        case ECMD_CALL: {
            dynamic_cast<TByteCodeCMDCall*>(command[it])->UpdateStack(exprs, allocator, defineVar, Define, command, it);
            break;
        }
        case ECMD_IFELSE: {
            //exprs.push_back(ParseCmdIfElse());
            break;
        }
        default:
            break;
        }
    }
    if (name == "define") {
        IdentType* ident = (IdentType*)stdFuncMap.at(name)(exprs);
        defineVar.insert(std::make_pair(ident->name, ident));
    }
    if (stdFuncMap.count(name)) {
        Stack.push_back(stdFuncMap.at(name)(exprs));
    } else if (Define.count(name)) {
        if (exprs.size() == Define.at(name)->Proto->Args.size()) {
            size_t j = 0;
            std::vector<ExprType*> result;
            std::map<std::string, IdentType* > defineVarBuffer = defineVar;
            std::map<std::string, FunctionType* > defineFuncBuffer = Define;
            for (auto i = Define.at(name)->Proto->Args.begin(); i != Define.at(name)->Proto->Args.end(); i++) {
                i->second->value = exprs.at(j);
                if (defineVar.count(i->first)) {
                    defineVar.erase(i->first);
                }
                defineVar.insert(std::make_pair(i->first, i->second));
                j++;
            }
            size_t current_position = it;
            it = Define.at(name)->Start;
            while (it != Define.at(name)->End) {
                switch ((ECommand)command[it]->Type) {
                case ECMD_PUSH: {
                    dynamic_cast<TByteCodeCMDPush*>(command[it])->UpdateStack(result, allocator);
                    break;
                }
                case ECMD_PUSHIDENT: {
                    dynamic_cast<TByteCodeCMDPushIdent*>(command[it])->UpdateStack(result, defineVar);
                    break;
                }
                case ECMD_CALL: {
                    dynamic_cast<TByteCodeCMDCall*>(command[it])->UpdateStack(result, allocator, defineVar, Define, command, it);
                    break;
                }
                case ECMD_IFELSE: {
                    //exprs.push_back(ParseCmdIfElse());
                    break;
                }
                case ECMD_TAILCALL: {
                    dynamic_cast<TByteCodeCMDTailCall*>(command[it])->UpdateStack(allocator, defineVar, Define, command, it);
                    break;
                }
                default:
                    break;
                }
                it++;
            }
            it = current_position;
            defineVar = defineVarBuffer;
            Define = defineFuncBuffer;
            Stack.push_back(result.at(result.size()-1));
        }
    }
}


TByteCodeCMDTailCall::TByteCodeCMDTailCall(std::string callee, size_t p)
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

void TByteCodeCMDTailCall::UpdateStack(std::vector<ExprType*> allocator, std::map<std::string, IdentType* > defineVar,
                                       std::map<std::string, FunctionType* > Define, std::vector<TByteCodeCMD*> command, size_t& it)
{
    std::vector<ExprType*> exprs;
    while (it < command.size()) {
        it++;
        if (command[it]->Type == ECMD_ENDCALL) {
            break;
        }
        switch (command[it]->Type) {
        case ECMD_PUSH: {
            dynamic_cast<TByteCodeCMDPush*>(command[it])->UpdateStack(exprs, allocator);
            break;
        }
        case ECMD_PUSHIDENT: {
            dynamic_cast<TByteCodeCMDPushIdent*>(command[it])->UpdateStack(exprs, defineVar);
            break;
        }
        case ECMD_CALL: {
            dynamic_cast<TByteCodeCMDCall*>(command[it])->UpdateStack(exprs, allocator, defineVar, Define, command, it);
            break;
        }
        case ECMD_IFELSE: {
            //exprs.push_back(ParseCmdIfElse());
            break;
        }
        case ECMD_TAILCALL: {
            //exprs.push_back(ParseCmdTailCall());
            break;
        }
        default:
            break;
        }
    }
    if (Define.count(name)) {
        if (exprs.size() == Define.at(name)->Proto->Args.size()) {
            size_t j = 0;
            for (auto i = Define.at(name)->Proto->Args.begin(); i != Define.at(name)->Proto->Args.end(); i++) {
                if (defineVar.count(i->first)) {
                    defineVar.at(i->first)->value = exprs.at(j);
                }
            }
            it = pos;
        }
    }
}


TByteCodeCMDAllocInt::TByteCodeCMDAllocInt(int val)
{
    Type = ECMD_AllOCINT;
    value = val;
}

void TByteCodeCMDAllocInt::UpdateStack(std::vector<ExprType *> &allocator)
{
    allocator.push_back(new NumberIntType(value));
}


TByteCodeCMDAllocDouble::TByteCodeCMDAllocDouble(double val)
{
    Type = ECMD_AllOCDOUBLE;
    value = val;
}

void TByteCodeCMDAllocDouble::UpdateStack(std::vector<ExprType *> &allocator)
{
    allocator.push_back(new NumberDoubleType(value));
}

TByteCodeCMDAllocChar::TByteCodeCMDAllocChar(char val)
{
    Type = ECMD_AllOCCHAR;
    value = val;
}

void TByteCodeCMDAllocChar::UpdateStack(std::vector<ExprType *> &allocator)
{
    allocator.push_back(new CharType(value));
}

TByteCodeCMDAllocString::TByteCodeCMDAllocString(std::string val)
{
    Type = ECMD_AllOCSTRING;
    value = val;
}

void TByteCodeCMDAllocString::UpdateStack(std::vector<ExprType *> &allocator)
{
    allocator.push_back(new StringType(value));
}

TByteCodeCMDAllocSymbol::TByteCodeCMDAllocSymbol(std::string val)
{
    Type = ECMD_AllOCSYMBOL;
    value = val;
}

void TByteCodeCMDAllocSymbol::UpdateStack(std::vector<ExprType *> &allocator)
{
    allocator.push_back(new SymbolType(value));
}

TByteCodeCMDAllocBool::TByteCodeCMDAllocBool(bool val)
{
    Type = ECMD_AllOCBOOL;
    value = val;
}

void TByteCodeCMDAllocBool::UpdateStack(std::vector<ExprType *> &allocator)
{
    allocator.push_back(new BoolType(value));
}


