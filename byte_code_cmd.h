#pragma once
#include <sstream>
#include "type.h"

enum ECommand {
    ECMD_AllOC = '0',
    ECMD_PUSH = '1',
    ECMD_PUSHIDENT = '2',
    ECMD_IFELSE = '3',
    ECMD_CALL = '4',
    ECMD_TAILCALL = '5',
    ECMD_DEFSTART = '6',
    ECMD_ENDCALL = '7',
    ECMD_ENDDEF = '8',
};

enum EValueType {
    EVT_INT = '0',
    EVT_DOUBLE = '1',
    EVT_STRING = '2',
    EVT_SYMBOL = '3',
    EVT_CHAR = '4',
    EVT_BOOL = '5'
};


class TByteCodeCMD {
public:
    virtual ~TByteCodeCMD(){}
    virtual void UpdateStack(){}
    ECommand Type;
};


class TByteCodeCMDAlloc: public TByteCodeCMD {
public:
    TByteCodeCMDAlloc(std::vector<ExprType*>* allocator, std::stringstream& str);
    virtual void UpdateStack();
    ECommand Type = ECMD_AllOC;
private:
    std::vector<ExprType*>* Allocator;
    std::stringstream& byteCodeString;
};

class TByteCodeCMDPush: public TByteCodeCMD {
public:
    TByteCodeCMDPush(std::vector<ExprType*>* stack, std::vector<ExprType*> allocator, std::stringstream& str);
    virtual void UpdateStack();
    ECommand Type = ECMD_PUSH;
private:
    std::vector<ExprType*>* Stack;
    std::vector<ExprType*> Allocator;
    std::stringstream& byteCodeString;
};

class TByteCodeCMDPushIdent: public TByteCodeCMD {
public:
    TByteCodeCMDPushIdent(std::vector<ExprType*>* stack, std::map<std::string, IdentType* > definevar, std::stringstream& str);
    virtual void UpdateStack();
    ECommand Type = ECMD_PUSHIDENT;
private:
    std::vector<ExprType*>* Stack;
    std::stringstream& byteCodeString;
    std::map<std::string, IdentType* > defineVar;
};

class TByteCodeCMDIfElse: public TByteCodeCMD {
public:
    TByteCodeCMDIfElse(std::vector<ExprType*>* stack, std::stringstream& str);
    virtual void UpdateStack();
    ECommand Type = ECMD_IFELSE;
private:
    std::vector<ExprType*>* Stack;
    std::stringstream& byteCodeString;
};

class TByteCodeCMDDefine: public TByteCodeCMD {
public:
    TByteCodeCMDDefine(std::map<std::string, FunctionType* >* define, std::stringstream& str);
    virtual void UpdateStack();
    ECommand Type = ECMD_DEFSTART;
private:
    std::map<std::string, FunctionType* >* Define;
    std::stringstream& byteCodeString;
};

class TByteCodeCMDCall: public TByteCodeCMD {
public:
    TByteCodeCMDCall(std::vector<ExprType*>* stack, std::vector<ExprType*> allocator, std::map<std::string, IdentType* > definevar,
                     std::map<std::string, FunctionType* > define, std::stringstream& str);
    virtual void UpdateStack();
    ECommand Type = ECMD_CALL;
private:
    std::vector<ExprType*>* Stack;
    std::vector<ExprType*> Allocator;
    std::map<std::string, FunctionType* > Define;
    std::map<std::string, IdentType* > defineVar;
    std::stringstream& byteCodeString;
    std::shared_ptr<TByteCodeCMD> currentCommand;
    std::map<std::string, ExprType*(*)(std::vector< ExprType* >) > stdFuncMap;
};

class TByteCodeCMDTailCall: public TByteCodeCMD {
public:
    TByteCodeCMDTailCall(std::vector<ExprType*>* stack, std::vector<ExprType *> allocator, std::map<std::string, IdentType *> definevar, std::map<std::string, FunctionType* > define,  std::stringstream& str);
    virtual void UpdateStack();
    ECommand Type = ECMD_TAILCALL;
private:
    std::vector<ExprType*>* Stack;
    std::vector<ExprType*> Allocator;
    std::map<std::string, FunctionType* > Define;
    std::map<std::string, IdentType* > defineVar;
    std::stringstream& byteCodeString;
    std::shared_ptr<TByteCodeCMD> currentCommand;
    std::map<std::string, ExprType*(*)(std::vector< ExprType* >) > stdFuncMap;
};

