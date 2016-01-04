#pragma once
#include <sstream>
#include "type.h"

enum ECommand {
    ECMD_AllOCINT = 0,
    ECMD_AllOCDOUBLE = 1,
    ECMD_AllOCSTRING = 2,
    ECMD_AllOCSYMBOL = 3,
    ECMD_AllOCCHAR = 4,
    ECMD_AllOCBOOL = 5,
    ECMD_PUSH = 6,
    ECMD_PUSHIDENT = 7,
    ECMD_IFELSE = 8,
    ECMD_CALL = 9,
    ECMD_TAILCALL = 10,
    ECMD_DEFSTART = 11,
    ECMD_ENDCALL = 12,
    ECMD_ENDDEF = 13
};

class TByteCodeCMD {
public:
    virtual ~TByteCodeCMD(){}
    virtual void UpdateStack(){}
    ECommand Type;
};

class TByteCodeCMDAllocInt: public TByteCodeCMD {
public:
    TByteCodeCMDAllocInt(int val);
    virtual void UpdateStack(std::vector<ExprType*>& allocator);
private:
    int value;
};

class TByteCodeCMDAllocDouble: public TByteCodeCMD {
public:
    TByteCodeCMDAllocDouble(double val);
    virtual void UpdateStack(std::vector<ExprType*>& allocator);
private:
    double value;
};

class TByteCodeCMDAllocChar: public TByteCodeCMD {
public:
    TByteCodeCMDAllocChar(char val);
    virtual void UpdateStack(std::vector<ExprType*>& allocator);
private:
    char value;
};

class TByteCodeCMDAllocString: public TByteCodeCMD {
public:
    TByteCodeCMDAllocString(std::string val);
    virtual void UpdateStack(std::vector<ExprType*>& allocator);
private:
    std::string value;
};

class TByteCodeCMDAllocSymbol: public TByteCodeCMD {
public:
    TByteCodeCMDAllocSymbol(std::string val);
    virtual void UpdateStack(std::vector<ExprType*>& allocator);
private:
    std::string value;
};

class TByteCodeCMDAllocBool: public TByteCodeCMD {
public:
    TByteCodeCMDAllocBool(bool val);
    virtual void UpdateStack(std::vector<ExprType*>& allocator);
private:
    bool value;
};

class TByteCodeCMDPush: public TByteCodeCMD {
public:
    TByteCodeCMDPush(size_t valNum);
    virtual void UpdateStack(std::vector<ExprType*>& stack, std::vector<ExprType*> allocator);
private:
    size_t valueNumber;
};

class TByteCodeCMDPushIdent: public TByteCodeCMD {
public:
    TByteCodeCMDPushIdent(std::string val);
    virtual void UpdateStack(std::vector<ExprType*>& stack, std::map<std::string, IdentType* > definevar);
private:
    std::string value;
};

class TByteCodeCMDIfElse: public TByteCodeCMD {
public:
    TByteCodeCMDIfElse(std::vector<ExprType*>* stack, std::stringstream& str);
    virtual void UpdateStack();
private:
    std::vector<ExprType*>* Stack;
    std::stringstream& byteCodeString;
};

class TByteCodeCMDDefine: public TByteCodeCMD {
public:
    TByteCodeCMDDefine(std::string name, size_t sizeArgs, std::vector<std::string> identName);
    virtual void UpdateStack(std::map<std::string, FunctionType* >& Define, std::vector<TByteCodeCMD*> command, size_t& it);
private:
    std::map<std::string, IdentType*> args;
    std::string funcName;
};

class TByteCodeCMDCall: public TByteCodeCMD {
public:
    TByteCodeCMDCall(std::string callee);
    virtual void UpdateStack(std::vector<ExprType*>& stack, std::vector<ExprType*> allocator, std::map<std::string, IdentType* > definevar,
                             std::map<std::string, FunctionType* > define, std::vector<TByteCodeCMD*> command, size_t& it);
private:
    std::string name;
    std::shared_ptr<TByteCodeCMD> currentCommand;
    std::map<std::string, ExprType*(*)(std::vector< ExprType* >) > stdFuncMap;
};

class TByteCodeCMDTailCall: public TByteCodeCMD {
public:
    TByteCodeCMDTailCall(std::string callee, size_t p);
    virtual void UpdateStack(std::vector<ExprType*> allocator, std::map<std::string, IdentType* > defineVar,
                             std::map<std::string, FunctionType* > Define, std::vector<TByteCodeCMD*> command, size_t& it);
private:
    std::string name;
    size_t pos;
    std::shared_ptr<TByteCodeCMD> currentCommand;
    std::map<std::string, ExprType*(*)(std::vector< ExprType* >) > stdFuncMap;
};


class TByteCodeCMDEndCall: public TByteCodeCMD {
public:
    TByteCodeCMDEndCall() {
        Type = ECMD_ENDCALL;
    }
};

class TByteCodeCMDEndDef: public TByteCodeCMD {
public:
    TByteCodeCMDEndDef() {
        Type = ECMD_ENDDEF;
    }
};
