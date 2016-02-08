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

public:
    int value;
};

class NumberDoubleType : public ExprType {

public:
    NumberDoubleType(double val) : value(val) {
        Type = T_Double;
    }

public:
    double value;
};


class IdentType : public ExprType {

public:
    IdentType(std::string val) : name(val) {
        Type = T_Ident;
    }
    ~IdentType() {
    }

public:
    std::string name;
    std::shared_ptr<ExprType> value;
};

class StringType : public ExprType {

public:
    StringType(std::string val) : value(val) {
        Type = T_String;
    }

public:
    std::string value;
};

class CharType : public ExprType {

public:
    CharType(char val) : value(val) {
        Type = T_Char;
    }

public:
    char value;
};

class BoolType : public ExprType {

public:
    BoolType(bool val) : value(val) {
        Type = T_Bool;
    }
    bool IsTrue() {
        return value;
    }

public:
    bool value;
};

class SymbolType : public ExprType {

public:
    SymbolType(std::string val) : value(val) {
        Type = T_Symbol;
    }

public:
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

public:
    std::shared_ptr<VList> value;
};

class CallExprType : public ExprType {
public:
  std::string Callee;
  std::vector< std::shared_ptr<ExprType> > Args;

public:
  CallExprType(const std::string &callee, std::vector< std::shared_ptr<ExprType> > &args)
    : Callee(callee), Args(args)
  {
      Type = T_CallFunc;
  }
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
    PrototypeType(const std::string name, const std::vector<std::string> args)
        : Name(name), Args(args)
    {
    }
public:
    std::string Name;
    std::vector<std::string> Args;
};

class Enviroment {
public:
    Enviroment(){}
    ~Enviroment(){}
    std::map<std::string, std::shared_ptr<ExprType>> This;
    Enviroment* Parent = nullptr;
};


class FunctionType: public ExprType {
public:
    FunctionType(PrototypeType* proto, size_t start, std::shared_ptr<Enviroment> e)
        : Proto(proto), Env(e)
    {
        for (size_t i = 0; i < Proto->Args.size();i++) {
            Env->This.insert(std::make_pair(Proto->Args[i], std::shared_ptr<ExprType>(new IdentType(Proto->Args[i]))));
        }
        Start = start;
        Type = T_Func;
    }
public:
    std::shared_ptr< PrototypeType >Proto;
    std::shared_ptr<Enviroment> Env;
    size_t Start;
    size_t End;
};
