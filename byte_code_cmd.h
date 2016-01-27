#pragma once
#include <sstream>
#include "type.h"
#include "stack.h"
#include "iostream"

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
    TByteCodeCMDAllocInt(int val, TStack& stack);
    void UpdateStack();
private:
    TStack& Stack;
    int value;
};

class TByteCodeCMDAllocDouble: public TByteCodeCMD {
public:
    TByteCodeCMDAllocDouble(double val, TStack& stack);
    void UpdateStack();
private:
    TStack& Stack;
    double value;
};

class TByteCodeCMDAllocChar: public TByteCodeCMD {
public:
    TByteCodeCMDAllocChar(char val, TStack& stack);
    void UpdateStack();
private:
    TStack& Stack;
    char value;
};

class TByteCodeCMDAllocString: public TByteCodeCMD {
public:
    TByteCodeCMDAllocString(std::string val, TStack& stack);
    void UpdateStack();
private:
    TStack& Stack;
    std::string value;
};

class TByteCodeCMDAllocSymbol: public TByteCodeCMD {
public:
    TByteCodeCMDAllocSymbol(std::string val, TStack& stack);
    void UpdateStack();
private:
    TStack& Stack;
    std::string value;
};

class TByteCodeCMDAllocBool: public TByteCodeCMD {
public:
    TByteCodeCMDAllocBool(bool val, TStack& stack);
    void UpdateStack();
private:
    TStack& Stack;
    bool value;
};

class TByteCodeCMDPush: public TByteCodeCMD {
public:
    TByteCodeCMDPush(size_t valNum, TStack& stack);
    void UpdateStack();
private:
    TStack& Stack;
    size_t valueNumber;
};

class TByteCodeCMDPushIdent: public TByteCodeCMD {
public:
    TByteCodeCMDPushIdent(std::string val, TStack& stack);
    void UpdateStack();
private:
    TStack& Stack;
    std::string value;
};

class TByteCodeCMDIfElse: public TByteCodeCMD {
public:
    TByteCodeCMDIfElse(TStack& stack, std::vector<std::shared_ptr<TByteCodeCMD>>& cmd, size_t& IT);
    void UpdateStack();
private:
    TStack& Stack;
    std::vector<std::shared_ptr<TByteCodeCMD>>& command;
    size_t& it;
    void Skip(std::vector<std::shared_ptr<TByteCodeCMD>> command, size_t& IT);
};

class TByteCodeCMDDefine: public TByteCodeCMD {
public:
    TByteCodeCMDDefine(std::string name, size_t sizeArgs, std::vector<std::string> identName,
                       TStack& stack, std::vector<std::shared_ptr<TByteCodeCMD>>& cmd, size_t& IT);
    void UpdateStack();
private:
    std::map<std::string, IdentType*> args;
    std::string funcName;
    TStack& Stack;
    std::vector<std::shared_ptr<TByteCodeCMD>>& command;
    size_t& it;
};

class TByteCodeCMDCall: public TByteCodeCMD {
public:
    TByteCodeCMDCall(std::string callee, TStack& stack, std::vector<std::shared_ptr<TByteCodeCMD>>& cmd, size_t& IT);
    void UpdateStack();
private:
    std::string name;
    TStack& Stack;
    std::vector<std::shared_ptr<TByteCodeCMD>>& command;
    size_t& it;
    std::map<std::string, ExprType*(*)(std::vector< std::shared_ptr<ExprType> > *) > stdFuncMap;
};

class TByteCodeCMDTailCall: public TByteCodeCMD {
public:
    TByteCodeCMDTailCall(std::string callee, size_t p, TStack& stack, std::vector<std::shared_ptr<TByteCodeCMD>>& cmd, size_t& IT);
    void UpdateStack();
private:
    std::string name;
    size_t pos;
    TStack& Stack;
    std::vector<std::shared_ptr<TByteCodeCMD>>& command;
    size_t& it;
    std::map<std::string, ExprType*(*)(std::vector<std::shared_ptr<ExprType> >*) > stdFuncMap;
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
