#include "byte_code_cmd.h"
#include <algorithm>
#include <iterator>
#include "scheme_std_lib.cpp"


TByteCodeCMDAlloc::TByteCodeCMDAlloc(std::vector<ExprType *> *allocator, std::stringstream &str): byteCodeString(str)  {
    Allocator = allocator;
}

void TByteCodeCMDAlloc::UpdateStack()
{
    char vtype;
    byteCodeString >> vtype;
    switch ((EValueType)vtype) {
    case EVT_INT: {
        int val;
        byteCodeString >> val;
        Allocator->push_back(new NumberIntType(val));
        break;
    }
    case EVT_DOUBLE: {
        double val;
        byteCodeString >> val;
        Allocator->push_back(new NumberDoubleType(val));
        break;
    }
    case EVT_STRING: {
        std::string val;
        byteCodeString >> val;
        Allocator->push_back(new StringType(val));
        break;
    }
    case EVT_SYMBOL: {
        std::string val;
        byteCodeString >> val;
        Allocator->push_back(new SymbolType(val));
        break;
    }
    case EVT_CHAR: {
        char val;
        byteCodeString >> val;
        Allocator->push_back(new CharType(val));
        break;
    }
    case EVT_BOOL: {
        bool val;
        byteCodeString >> val;
        Allocator->push_back(new BoolType(val));
        break;
    }
    default:
        break;
    }
}


TByteCodeCMDPush::TByteCodeCMDPush(std::vector<ExprType *> *stack, std::vector<ExprType *> allocator, std::stringstream &str): byteCodeString(str) {
    Stack = stack;
    Allocator = allocator;
}

void TByteCodeCMDPush::UpdateStack()
{
    size_t val;
    byteCodeString >> val;
    Stack->push_back(Allocator.at(val));
}


TByteCodeCMDPushIdent::TByteCodeCMDPushIdent(std::vector<ExprType *> *stack, std::map<std::string, IdentType *> definevar, std::stringstream &str): byteCodeString(str) {
    Stack = stack;
    defineVar = definevar;
}

void TByteCodeCMDPushIdent::UpdateStack()
{
    std::string val;
    byteCodeString >> val;
    Stack->push_back(defineVar.at(val)->value);
}


TByteCodeCMDIfElse::TByteCodeCMDIfElse(std::vector<ExprType *> *stack, std::stringstream &str): byteCodeString(str) {
    Stack = stack;
}

void TByteCodeCMDIfElse::UpdateStack()
{

}


TByteCodeCMDDefine::TByteCodeCMDDefine(std::map<std::string, FunctionType *> *define, std::stringstream &str): byteCodeString(str) {
    Define = define;
}

void TByteCodeCMDDefine::UpdateStack()
{
    std::string funcName;
    size_t sizeArgs;
    byteCodeString >> funcName >> sizeArgs;
    std::map<std::string, IdentType*> args;
    for (size_t i = 0; i < sizeArgs; i++) {
        std::string identName;
        byteCodeString >> identName;
        args.insert(std::make_pair(identName, new IdentType(identName)));
    }
    PrototypeType* proto = new PrototypeType(funcName, args);
    auto start = byteCodeString.tellg();
    while (!byteCodeString.eof()) {
        char cmd;
        byteCodeString >> cmd;
        if ((ECommand)cmd == ECMD_ENDDEF) {
            auto end = byteCodeString.tellg();
            Define->insert(std::make_pair(funcName ,new FunctionType(proto, start, end)));
            break;
        }
    }
}


TByteCodeCMDCall::TByteCodeCMDCall(std::vector<ExprType *> *stack,
                                   std::vector<ExprType *> allocator,
                                   std::map<std::string, IdentType *> definevar,
                                   std::map<std::string, FunctionType *> define,
                                   std::stringstream &str): byteCodeString(str)
{
    Stack = stack;
    Allocator = allocator;
    Define = define;
    defineVar = definevar;
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
    std::string name;
    byteCodeString >> name;
    std::vector<ExprType*> exprs;
    while (!byteCodeString.eof()) {
        char cmd;
        byteCodeString >> cmd;
        if ((ECommand)cmd == ECMD_ENDCALL) {
            break;
        }
        switch ((ECommand)cmd) {
        case ECMD_PUSH: {
            currentCommand.reset(new TByteCodeCMDPush(&exprs, Allocator, byteCodeString));
            currentCommand->UpdateStack();
            break;
        }
        case ECMD_PUSHIDENT: {
            currentCommand.reset(new TByteCodeCMDPushIdent(&exprs, defineVar, byteCodeString));
            currentCommand->UpdateStack();
            break;
        }
        case ECMD_CALL: {
            currentCommand.reset(new TByteCodeCMDCall(&exprs, Allocator, defineVar, Define, byteCodeString));
            currentCommand->UpdateStack();
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
    if (name == "define") {
        IdentType* ident = (IdentType*)stdFuncMap.at(name)(exprs);
        defineVar.insert(std::make_pair(ident->name, ident));
    }
    if (stdFuncMap.count(name)) {
        Stack->push_back(stdFuncMap.at(name)(exprs));
    } else if (Define.count(name)) {
        if (exprs.size() == Define.at(name)->Proto->Args.size()) {
            size_t j = 0;
            std::vector<ExprType*> result;
            std::map<std::string, IdentType* > defineVarBuffer = defineVar;
            std::map<std::string, FunctionType* > defineFuncBuffer = Define;
            for (auto it = Define.at(name)->Proto->Args.begin(); it != Define.at(name)->Proto->Args.end(); it++) {
                it->second->value = exprs.at(j);
                if (defineVar.count(it->first)) {
                    defineVar.erase(it->first);
                }
                defineVar.insert(std::make_pair(it->first, it->second));
                j++;
            }
            std::stringstream::pos_type current_position = byteCodeString.tellg();
            byteCodeString.seekg(Define.at(name)->Start);
            std::cout << std::endl << (int)Define.at(name)->Start << std::endl;
            while (byteCodeString.tellg() != Define.at(name)->End) {
                char cmd;
                byteCodeString >> cmd;
                switch ((ECommand)cmd) {
                case ECMD_PUSH: {
                    currentCommand.reset(new TByteCodeCMDPush(&result, Allocator, byteCodeString));
                    currentCommand->UpdateStack();
                    break;
                }
                case ECMD_PUSHIDENT: {
                    currentCommand.reset(new TByteCodeCMDPushIdent(&result, defineVar, byteCodeString));
                    currentCommand->UpdateStack();
                    break;
                }
                case ECMD_CALL: {
                    currentCommand.reset(new TByteCodeCMDCall(&result, Allocator, defineVar, Define, byteCodeString));
                    currentCommand->UpdateStack();
                    break;
                }
                case ECMD_IFELSE: {
                    //exprs.push_back(ParseCmdIfElse());
                    break;
                }
                case ECMD_TAILCALL: {
                    currentCommand.reset(new TByteCodeCMDTailCall(&result, Allocator, defineVar, Define, byteCodeString));
                    currentCommand->UpdateStack();
                    break;
                }
                default:
                    break;
                }
            }
            byteCodeString.seekg(current_position);
            defineVar = defineVarBuffer;
            Define = defineFuncBuffer;
            Stack->push_back(result.at(result.size()-1));
        }
    }
}


TByteCodeCMDTailCall::TByteCodeCMDTailCall(std::vector<ExprType *> *stack,
                                           std::vector<ExprType *> allocator,
                                           std::map<std::string,IdentType *> definevar,
                                           std::map<std::string, FunctionType *> define,
                                           std::stringstream &str): byteCodeString(str)
{
    Stack = stack;
    defineVar = definevar;
    Allocator = allocator;
    Define = define;
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
    std::string name;
    int pos;
    byteCodeString >> name >> pos;
    std::vector<ExprType*> exprs;
    while (!byteCodeString.eof()) {
        char cmd;
        byteCodeString >> cmd;
        if ((ECommand)cmd == ECMD_ENDCALL) {
            break;
        }
        switch ((ECommand)cmd) {
        case ECMD_PUSH: {
            currentCommand.reset(new TByteCodeCMDPush(&exprs, Allocator, byteCodeString));
            currentCommand->UpdateStack();
            break;
        }
        case ECMD_PUSHIDENT: {
            currentCommand.reset(new TByteCodeCMDPushIdent(&exprs, defineVar, byteCodeString));
            currentCommand->UpdateStack();
            break;
        }
        case ECMD_CALL: {
            currentCommand.reset(new TByteCodeCMDCall(&exprs, Allocator, defineVar, Define, byteCodeString));
            currentCommand->UpdateStack();
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
            for (auto it = Define.at(name)->Proto->Args.begin(); it != Define.at(name)->Proto->Args.end(); it++) {
                if (defineVar.count(it->first)) {
                    defineVar.at(it->first)->value = exprs.at(j);
                }
            }
            byteCodeString.seekg((std::stringstream::pos_type)pos);
        }
    }
}
