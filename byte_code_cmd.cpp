#include "byte_code_cmd.h"
#include <algorithm>
#include <iterator>
#include "scheme_std_lib.cpp"

TByteCodeCMDPush::TByteCodeCMDPush(size_t valNum) {
    Type = ECMD_PUSH;
    valueNumber = valNum;
}

void TByteCodeCMDPush::UpdateStack(std::vector<std::shared_ptr<ExprType>>& stack, std::vector<std::shared_ptr<ExprType>> allocator)
{
    stack.push_back(allocator.at(valueNumber));
}


TByteCodeCMDPushIdent::TByteCodeCMDPushIdent(std::string val) {
    Type = ECMD_PUSHIDENT;
    value = val;
}

void TByteCodeCMDPushIdent::UpdateStack(std::vector<std::shared_ptr<ExprType>>& stack, std::map<std::string, std::shared_ptr<IdentType> > defineVar)
{
    stack.push_back(defineVar.at(value)->value);
}


TByteCodeCMDIfElse::TByteCodeCMDIfElse() {
    Type = ECMD_IFELSE;
}

void TByteCodeCMDIfElse::UpdateStack(std::vector<std::shared_ptr<ExprType>>& stack, std::vector<std::shared_ptr<ExprType>> allocator, std::map<std::string, std::shared_ptr<IdentType> > defineVar,
                                     std::map<std::string, std::shared_ptr<FunctionType> > Define, std::vector<std::shared_ptr<TByteCodeCMD>> command, size_t& it)
{
    it++;
    std::vector<std::shared_ptr<ExprType>> exprs;
    switch (command[it]->Type) {
    case ECMD_PUSH: {
        dynamic_cast<TByteCodeCMDPush*>(command[it].get())->UpdateStack(exprs, allocator);
        break;
    }
    case ECMD_PUSHIDENT: {
        dynamic_cast<TByteCodeCMDPushIdent*>(command[it].get())->UpdateStack(exprs, defineVar);
        break;
    }
    case ECMD_CALL: {
        dynamic_cast<TByteCodeCMDCall*>(command[it].get())->UpdateStack(exprs, allocator, defineVar, Define, command, it);
        break;
    }
    case ECMD_TAILCALL: {
        dynamic_cast<TByteCodeCMDTailCall*>(command[it].get())->UpdateStack(allocator, defineVar, Define, command, it);
        return;
    }
    default:
        break;
    }
    it++;
    if (exprs.at(0)->Type == T_Bool && !(dynamic_cast<BoolType*>(exprs.at(0).get())->value)) {
        Skip(command, it);
        if (command.at(it)->Type == ECMD_ENDCALL) {
            return;
        } else {
            switch (command[it]->Type) {
            case ECMD_PUSH: {
                dynamic_cast<TByteCodeCMDPush*>(command[it].get())->UpdateStack(exprs, allocator);
                break;
            }
            case ECMD_PUSHIDENT: {
                dynamic_cast<TByteCodeCMDPushIdent*>(command[it].get())->UpdateStack(exprs, defineVar);
                break;
            }
            case ECMD_CALL: {
                dynamic_cast<TByteCodeCMDCall*>(command[it].get())->UpdateStack(exprs, allocator, defineVar, Define, command, it);
                break;
            }
            case ECMD_IFELSE: {
                dynamic_cast<TByteCodeCMDIfElse*>(command[it].get())->UpdateStack(exprs, allocator, defineVar, Define, command, it);
                break;
            }
            case ECMD_TAILCALL: {
                dynamic_cast<TByteCodeCMDTailCall*>(command[it].get())->UpdateStack(allocator, defineVar, Define, command, it);
                return;
            }
            default:
                break;
            }
        }
    } else {
        switch (command[it]->Type) {
        case ECMD_PUSH: {
            dynamic_cast<TByteCodeCMDPush*>(command[it].get())->UpdateStack(exprs, allocator);
            break;
        }
        case ECMD_PUSHIDENT: {
            dynamic_cast<TByteCodeCMDPushIdent*>(command[it].get())->UpdateStack(exprs, defineVar);
            break;
        }
        case ECMD_CALL: {
            dynamic_cast<TByteCodeCMDCall*>(command[it].get())->UpdateStack(exprs, allocator, defineVar, Define, command, it);
            break;
        }
        case ECMD_IFELSE: {
            dynamic_cast<TByteCodeCMDIfElse*>(command[it].get())->UpdateStack(exprs, allocator, defineVar, Define, command, it);
            break;
        }
        case ECMD_TAILCALL: {
            dynamic_cast<TByteCodeCMDTailCall*>(command[it].get())->UpdateStack(allocator, defineVar, Define, command, it);
            return;
        }
        default:
            break;
        }
    }
    stack.push_back(exprs.at(exprs.size()-1));
    it++;
    if (command.at(it)->Type == ECMD_ENDCALL) {
        return;
    } else {
        Skip(command, it);
    }
}

void TByteCodeCMDIfElse::Skip(std::vector<std::shared_ptr<TByteCodeCMD> > command, size_t &it)
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


TByteCodeCMDDefine::TByteCodeCMDDefine(std::string name, size_t size, std::vector<std::string> idents) {

    Type = ECMD_DEFSTART;
    funcName = name;
    size_t sizeArgs = size;
    for (size_t i = 0; i < sizeArgs; i++) {
        args.insert(std::make_pair(idents[i], new IdentType(idents[i])));
    }
}

void TByteCodeCMDDefine::UpdateStack(std::map<std::string, std::shared_ptr<FunctionType> >& Define, std::vector<std::shared_ptr<TByteCodeCMD>> command, size_t& it)
{
    PrototypeType* proto = new PrototypeType(funcName, args);
    size_t start = it;
    while (it < command.size()) {
        it++;
        if ((ECommand)command[it]->Type == ECMD_ENDDEF) {
            size_t end = it;
            Define.insert(std::make_pair(funcName ,std::shared_ptr<FunctionType>(new FunctionType(proto, start, end))));
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

void TByteCodeCMDCall::UpdateStack(std::vector<std::shared_ptr<ExprType>>& Stack, std::vector<std::shared_ptr<ExprType>> allocator, std::map<std::string, std::shared_ptr<IdentType> > defineVar,
                                   std::map<std::string, std::shared_ptr<FunctionType> > Define, std::vector<std::shared_ptr<TByteCodeCMD>> command, size_t& it)
{
    std::vector<std::shared_ptr<ExprType>> exprs;
    while (it < command.size()) {
        it++;
        if (command[it]->Type == ECMD_ENDCALL) {
            break;
        }
        switch (command[it]->Type) {
        case ECMD_PUSH: {
            dynamic_cast<TByteCodeCMDPush*>(command[it].get())->UpdateStack(exprs, allocator);
            break;
        }
        case ECMD_PUSHIDENT: {
            dynamic_cast<TByteCodeCMDPushIdent*>(command[it].get())->UpdateStack(exprs, defineVar);
            break;
        }
        case ECMD_CALL: {
            dynamic_cast<TByteCodeCMDCall*>(command[it].get())->UpdateStack(exprs, allocator, defineVar, Define, command, it);
            break;
        }
        case ECMD_IFELSE: {
            dynamic_cast<TByteCodeCMDIfElse*>(command[it].get())->UpdateStack(exprs, allocator, defineVar, Define, command, it);
            break;
        }
        default:
            break;
        }
    }
    if (name == "define") {
        IdentType* ident = (IdentType*)stdFuncMap.at(name)(&exprs);
        defineVar.insert(std::make_pair(ident->name, std::shared_ptr<IdentType>(ident)));
    }
    if (stdFuncMap.count(name)) {
        Stack.push_back(std::shared_ptr<ExprType>(stdFuncMap.at(name)(&exprs)));
    } else if (Define.count(name)) {
        if (exprs.size() == Define.at(name)->Proto->Args.size()) {
            size_t j = 0;
            std::vector<std::shared_ptr<ExprType>> result;
            std::map<std::string, std::shared_ptr<IdentType> > defineVarBuffer = defineVar;
            std::map<std::string, std::shared_ptr<FunctionType> > defineFuncBuffer = Define;
            for (auto i = Define.at(name)->Proto->Args.begin(); i != Define.at(name)->Proto->Args.end(); i++) {
                i->second->value.reset();
                i->second->value = exprs.at(j);
                if (defineVar.count(i->first)) {
                    defineVar.erase(i->first);
                }
                defineVar.insert(std::make_pair(i->first,std::shared_ptr<IdentType>(i->second)));
                j++;
            }
            size_t current_position = it;
            it = Define.at(name)->Start;
            while (it != Define.at(name)->End) {
                switch ((ECommand)command[it]->Type) {
                case ECMD_PUSH: {
                    dynamic_cast<TByteCodeCMDPush*>(command[it].get())->UpdateStack(result, allocator);
                    break;
                }
                case ECMD_PUSHIDENT: {
                    dynamic_cast<TByteCodeCMDPushIdent*>(command[it].get())->UpdateStack(result, defineVar);
                    break;
                }
                case ECMD_CALL: {
                    dynamic_cast<TByteCodeCMDCall*>(command[it].get())->UpdateStack(result, allocator, defineVar, Define, command, it);
                    break;
                }
                case ECMD_IFELSE: {
                    dynamic_cast<TByteCodeCMDIfElse*>(command[it].get())->UpdateStack(result, allocator, defineVar, Define, command, it);
                    break;
                }
                case ECMD_TAILCALL: {
                    result.clear();
                    dynamic_cast<TByteCodeCMDTailCall*>(command[it].get())->UpdateStack(allocator, defineVar, Define, command, it);
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

void TByteCodeCMDTailCall::UpdateStack(std::vector<std::shared_ptr<ExprType>> allocator, std::map<std::string, std::shared_ptr<IdentType> > defineVar,
                                       std::map<std::string, std::shared_ptr<FunctionType> > Define, std::vector<std::shared_ptr<TByteCodeCMD>> command, size_t& it)
{
    std::vector<std::shared_ptr<ExprType>> exprs;
    while (it < command.size()) {
        it++;
        if (command[it]->Type == ECMD_ENDCALL) {
            break;
        }
        switch (command[it]->Type) {
        case ECMD_PUSH: {
            dynamic_cast<TByteCodeCMDPush*>(command[it].get())->UpdateStack(exprs, allocator);
            break;
        }
        case ECMD_PUSHIDENT: {
            dynamic_cast<TByteCodeCMDPushIdent*>(command[it].get())->UpdateStack(exprs, defineVar);
            break;
        }
        case ECMD_CALL: {
            dynamic_cast<TByteCodeCMDCall*>(command[it].get())->UpdateStack(exprs, allocator, defineVar, Define, command, it);
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
                    defineVar.at(i->first)->value.reset();
                    defineVar.at(i->first)->value = exprs.at(j);
                }
                j++;
            }
            it = pos-1;
        }
    }
}


TByteCodeCMDAllocInt::TByteCodeCMDAllocInt(int val)
{
    Type = ECMD_AllOCINT;
    value = val;
}

void TByteCodeCMDAllocInt::UpdateStack(std::vector<std::shared_ptr<ExprType>> &allocator)
{
    allocator.push_back(std::shared_ptr<ExprType>(new NumberIntType(value)));
}


TByteCodeCMDAllocDouble::TByteCodeCMDAllocDouble(double val)
{
    Type = ECMD_AllOCDOUBLE;
    value = val;
}

void TByteCodeCMDAllocDouble::UpdateStack(std::vector<std::shared_ptr<ExprType>> &allocator)
{
    allocator.push_back(std::shared_ptr<ExprType>(new NumberDoubleType(value)));
}

TByteCodeCMDAllocChar::TByteCodeCMDAllocChar(char val)
{
    Type = ECMD_AllOCCHAR;
    value = val;
}

void TByteCodeCMDAllocChar::UpdateStack(std::vector<std::shared_ptr<ExprType>> &allocator)
{
    allocator.push_back(std::shared_ptr<ExprType>(new CharType(value)));
}

TByteCodeCMDAllocString::TByteCodeCMDAllocString(std::string val)
{
    Type = ECMD_AllOCSTRING;
    value = val;
}

void TByteCodeCMDAllocString::UpdateStack(std::vector<std::shared_ptr<ExprType>> &allocator)
{
    allocator.push_back(std::shared_ptr<ExprType>(new StringType(value)));
}

TByteCodeCMDAllocSymbol::TByteCodeCMDAllocSymbol(std::string val)
{
    Type = ECMD_AllOCSYMBOL;
    value = val;
}

void TByteCodeCMDAllocSymbol::UpdateStack(std::vector<std::shared_ptr<ExprType>> &allocator)
{
    allocator.push_back(std::shared_ptr<ExprType>(new SymbolType(value)));
}

TByteCodeCMDAllocBool::TByteCodeCMDAllocBool(bool val)
{
    Type = ECMD_AllOCBOOL;
    value = val;
}

void TByteCodeCMDAllocBool::UpdateStack(std::vector<std::shared_ptr<ExprType>> &allocator)
{
    allocator.push_back(std::shared_ptr<ExprType>(new BoolType(value)));
}


