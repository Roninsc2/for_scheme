#pragma once
#include <sstream>
#include "type.h"
#include "stack.h"
#include "iostream"


class TByteCodeCMD {
public:
    virtual ~TByteCodeCMD(){}
    virtual void UpdateStack(TStack& Stack){}
};

class TByteCodeCMDAllocInt: public TByteCodeCMD {
public:
    TByteCodeCMDAllocInt(int val);
    void UpdateStack(TStack& Stack);
private:
    int value;
};

class TByteCodeCMDAllocDouble: public TByteCodeCMD {
public:
    TByteCodeCMDAllocDouble(double val);
    void UpdateStack(TStack& Stack);
private:
    double value;
};

class TByteCodeCMDAllocChar: public TByteCodeCMD {
public:
    TByteCodeCMDAllocChar(char val);
    void UpdateStack(TStack& Stack);
private:
    char value;
};

class TByteCodeCMDAllocString: public TByteCodeCMD {
public:
    TByteCodeCMDAllocString(std::string val);
    void UpdateStack(TStack& Stack);
private:  
    std::string value;
};

class TByteCodeCMDAllocSymbol: public TByteCodeCMD {
public:
    TByteCodeCMDAllocSymbol(std::string val);
    void UpdateStack(TStack& Stack);
private:
    std::string value;
};

class TByteCodeCMDAllocBool: public TByteCodeCMD {
public:
    TByteCodeCMDAllocBool(bool val);
    void UpdateStack(TStack& Stack);
private:
    bool value;
};

class TByteCodeCMDPush: public TByteCodeCMD {
public:
    TByteCodeCMDPush(size_t valNum);
    void UpdateStack(TStack& Stack);
private:
    size_t valueNumber;
};

class TByteCodeCMDPushIdent: public TByteCodeCMD {
public:
    TByteCodeCMDPushIdent(std::string val);
    void UpdateStack(TStack& Stack);
private:
    std::string value;
};

class TByteCodeCMDIfElse: public TByteCodeCMD {
public:
    TByteCodeCMDIfElse(std::vector<std::shared_ptr<TByteCodeCMD>>& cmd, size_t& IT);
    void UpdateStack(TStack& Stack);
private:
    std::vector<std::shared_ptr<TByteCodeCMD>>& command;
    size_t& it;
    void Skip(std::vector<std::shared_ptr<TByteCodeCMD>> command, size_t& IT);
};

class TByteCodeCMDDefine: public TByteCodeCMD {
public:
    TByteCodeCMDDefine(std::string name, size_t sizeArgs, std::vector<std::string> identName,
                       std::vector<std::shared_ptr<TByteCodeCMD>>& cmd, size_t& IT);
    void UpdateStack(TStack& Stack);
private:
    std::map<std::string, std::shared_ptr<IdentType>> args;
    std::string funcName;
    std::vector<std::shared_ptr<TByteCodeCMD>>& command;
    size_t& it;
};

class TByteCodeCMDCall: public TByteCodeCMD {
public:
    TByteCodeCMDCall(std::string callee, size_t i, size_t& IT, std::vector<std::shared_ptr<TByteCodeCMD>>& Command);
    void UpdateStack(TStack& Stack);
private:
    std::string name;
    size_t size;
    size_t& it;
    std::vector<std::shared_ptr<TByteCodeCMD>>& command;
    std::map<std::string, ExprType*(*)(std::vector< std::shared_ptr<ExprType> >&) > stdFuncMap;
};

class TByteCodeCMDTailCall: public TByteCodeCMD {
public:
    TByteCodeCMDTailCall(std::string callee, size_t p, size_t i, size_t& IT);
    void UpdateStack(TStack& Stack);
private:
    std::string name;
    size_t pos;
    size_t size;
    size_t& it;
    std::map<std::string, ExprType*(*)(std::vector<std::shared_ptr<ExprType> >&) > stdFuncMap;
};


class TByteCodeCMDEndCall: public TByteCodeCMD {
public:
    TByteCodeCMDEndCall() {
    }
};

class TByteCodeCMDEndDef: public TByteCodeCMD {
public:
    TByteCodeCMDEndDef(size_t& IT): it(IT)
    {
    }
    void UpdateStack(TStack& Stack) {
        Stack.defineFunc = defineFunctionBuffer;
        Stack.defineVar = defineVaribleBuffer;
        it = currentPos.at(currentPos.size()-1);
        currentPos.pop_back();
    }
    std::vector<size_t> currentPos;
    size_t& it;
    std::map<std::string, std::shared_ptr<IdentType> > defineVaribleBuffer;
    std::map<std::string, std::shared_ptr<FunctionType> > defineFunctionBuffer;
};
