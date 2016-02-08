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
    TByteCodeCMDPushIdent(std::string val, Enviroment& defineParent);
    void UpdateStack(TStack& Stack);
private:
    Enviroment& defineParent;
    std::string value;
};

class TByteCodeCMDIfElse: public TByteCodeCMD {
public:
    TByteCodeCMDIfElse(size_t& IT);
    void UpdateStack(TStack& Stack);
private:
    size_t& it;
};

class TByteCodeCMDLambda: public TByteCodeCMD {
public:
    TByteCodeCMDLambda(std::vector<std::string> agrs, std::vector<std::shared_ptr<TByteCodeCMD>>& cmd);
    void UpdateStack(TStack& Stack);
    size_t end;
private:
    std::vector<std::shared_ptr<TByteCodeCMD>>& command;
    std::map<std::string, std::shared_ptr<IdentType>> args;
};

class TByteCodeCMDDefine: public TByteCodeCMD {
public:
    TByteCodeCMDDefine(std::string name, std::vector<std::string> identName,
                       Enviroment& defParent,
                       std::vector<std::shared_ptr<TByteCodeCMD>>& cmd, size_t& IT);
    void UpdateStack(TStack& Stack);
public:
    std::shared_ptr<Enviroment> define = std::shared_ptr<Enviroment>(new Enviroment());
private:
    std::vector<std::string> Args;
    std::string funcName;
    std::vector<std::shared_ptr<TByteCodeCMD>>& command;
    size_t& it;
};

class TByteCodeCMDCall: public TByteCodeCMD {
public:
    TByteCodeCMDCall(std::string callee, size_t i, size_t& IT, std::vector<std::shared_ptr<TByteCodeCMD>>& Command, Enviroment& defParent);
    void UpdateStack(TStack& Stack);
private:
    std::string name;
    size_t size;
    size_t& it;
    std::vector<std::shared_ptr<TByteCodeCMD>>& command;
    Enviroment& defineParent;
};

class TByteCodeCMDTailCall: public TByteCodeCMD {
public:
    TByteCodeCMDTailCall(std::string callee, size_t p, size_t i, size_t& IT, Enviroment& defParent);
    void UpdateStack(TStack& Stack);
private:
    std::string name;
    size_t pos;
    size_t size;
    size_t& it;
    Enviroment& defineParent;
};

class TByteCodeCMDJump: public TByteCodeCMD {
public:
    TByteCodeCMDJump(size_t& It): it(It) {
    }
    void UpdateStack(TStack& Stack) {
        it = currentPos;
    }

private:
    size_t& it;
public:
    size_t currentPos;
};

class TByteCodeCMDJumpEndDefine: public TByteCodeCMD {
public:
    TByteCodeCMDJumpEndDefine(size_t& It): it(It) {
    }
    void UpdateStack(TStack& Stack) {
        it = currentPos;
    }

private:
    size_t& it;
public:
    size_t currentPos;
};
