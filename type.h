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
  NoneType()
  {
      Type = T_None;
  }
};



class PrototypeType {
public:
    PrototypeType(const std::string &name, const std::map<std::string, IdentType*> &args)
        : Name(name), Args(args)
    {
    }
public:
    std::string Name;
    std::map<std::string, IdentType*> Args;
};


class FunctionType: public ExprType {
public:
    FunctionType(PrototypeType* proto, size_t start, size_t end)
        : Proto(proto)
    {
        Start = start;
        End = end;
        Type = T_Func;
    }
public:
    std::shared_ptr< PrototypeType >Proto;
    size_t Start;
    size_t End;
};
