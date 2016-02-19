#pragma once
#include <string>
#include <vector>
#include <memory>
#include <vlist.h>
#include <map>
#include <sstream>
#include <iterator>
#include <algorithm>


enum EType {
    T_Int,//0
    T_Double,//1
    T_String,//2
    T_Symbol,//3
    T_Char,//4
    T_Bool,//5
    T_List, //6
    T_Ident,//7
    T_Func,//8
    T_CallFunc,//9
    T_None
};

class ExprType {
public:
    virtual ~ExprType(){}
    virtual bool IsTrue(){
        return true;
    }

    EType Type;
};

class NumberIntType : public ExprType {

public:
    NumberIntType(int val) : value(val) {
        Type = T_Int;
    }
    int GetValue();

private:
    int value;
};

class NumberDoubleType : public ExprType {

public:
    NumberDoubleType(double val) : value(val) {
        Type = T_Double;
    }
    double GetValue();

private:
    double value;
};


class IdentType : public ExprType {

public:
    IdentType(std::string val) : name(val) {
        Type = T_Ident;
    }
    ~IdentType() {
    }
    std::shared_ptr<ExprType>& GetValue();
    void ResetValue(std::shared_ptr<ExprType>& expr);

private:
    std::string name;
    std::shared_ptr<ExprType> value;
};

class StringType : public ExprType {

public:
    StringType(std::string val) : value(val) {
        Type = T_String;
    }
    std::string GetValue();

private:
    std::string value;
};

class CharType : public ExprType {

public:
    CharType(char val) : value(val) {
        Type = T_Char;
    }
    char GetValue();

private:
    char value;
};

class BoolType : public ExprType {

public:
    BoolType(bool val) : value(val) {
        Type = T_Bool;
    }
    bool IsTrue();
    bool GetValue();

private:
    bool value;
};

class SymbolType : public ExprType {

public:
    SymbolType(std::string val) : value(val) {
        Type = T_Symbol;
    }
    std::string GetValue();

private:
    std::string value;
};

class ListType : public ExprType {

public:
    ListType(VList* val) : value(val) {
        Type = T_List;
    }
    ListType(std::shared_ptr<VList> val) : value(val) {
        Type = T_List;
    }
    VList* GetValue();

private:
    std::shared_ptr<VList> value;
};

class NoneType : public ExprType {
public:
  NoneType() {
      Type = T_None;
  }
  bool IsTrue() {
    return false;
  }
};



class PrototypeType {
public:
    PrototypeType(const std::vector<std::string> args)
        :  Args(args)
    {
    }
    std::string& GetArgsAt(size_t i);
    size_t GetArgsSize();

private:
    std::vector<std::string> Args;
};

class Enviroment {
public:
    Enviroment(){}
    ~Enviroment(){}
    std::map<std::string, std::shared_ptr<ExprType>> GetEnviroment();
    Enviroment* GetParent();
    void ResetParent(Enviroment* env);
    void InsertToEnviroment(std::string name, std::shared_ptr<ExprType> expr);
    ExprType* Find(std::string name);
    Enviroment* FindVarible(std::string name);
private:
    std::map<std::string, std::shared_ptr<ExprType>> This;
    Enviroment* Parent = nullptr;
};


class FunctionType: public ExprType {
public:
    FunctionType(PrototypeType* proto, size_t start, std::shared_ptr<Enviroment> e)
        : Proto(proto), Env(e)
    {
        for (size_t i = 0; i < Proto->GetArgsSize();i++) {
            Env->InsertToEnviroment(Proto->GetArgsAt(i), std::shared_ptr<ExprType>(new IdentType(Proto->GetArgsAt(i))));
        }
        Start = start;
        Type = T_Func;
    }
    size_t GetStart();
    void ResetStart(size_t i);
    size_t GetEnd();
    void ResetEnd(size_t i);
    PrototypeType* GetPrototype();
    Enviroment* GetEnviroment();

private:
    std::shared_ptr< PrototypeType >Proto;
    std::shared_ptr<Enviroment> Env;
    size_t Start;
    size_t End;
};
