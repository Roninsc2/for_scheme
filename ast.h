#pragma once
#include <string>
#include <vector>
#include <memory>



enum EAstType {
    AT_Int,//0
    AT_Double,//1
    AT_String,//2
    AT_Symbol,//3
    AT_Char,//4
    AT_Bool,//5
    AT_List, //6
    AT_Ident,//7
    AT_Func
};

class ExprAST {
public:
    virtual ~ExprAST(){}
    EAstType Type;
};

class NumberIntAST : public ExprAST {

public:
    NumberIntAST(int val) : value(val) {
        Type = AT_Int;
    }

public:
    int value;
};

class NumberDoubleAST : public ExprAST {

public:
    NumberDoubleAST(double val) : value(val) {
        Type = AT_Double;
    }

public:
    double value;
};


class IdentAST : public ExprAST {

public:
    IdentAST(std::string val) : name(val) {
        Type = AT_Ident;
    }
public:
    std::string name;
};

class StringAST : public ExprAST {

public:
    StringAST(std::string val) : value(val) {
        Type = AT_String;
    }

public:
    std::string value;
};

class CharAST : public ExprAST {

public:
    CharAST(char val) : value(val) {
        Type = AT_Char;
    }

public:
    char value;
};

class BoolAST : public ExprAST {

public:
    BoolAST(bool val) : value(val) {
        Type = AT_Bool;
    }

public:
    bool value;
};

class SymbolAST : public ExprAST {

public:
    SymbolAST(std::string val) : value(val) {
        Type = AT_Symbol;
    }

public:
    std::string value;
};

class CallExprAST : public ExprAST {
public:
  std::string Callee;
  std::vector< ExprAST* > Args;

public:
  CallExprAST(const std::string &callee, std::vector< ExprAST* > &args)
    : Callee(callee), Args(args)
  {
      Type = AT_Func;
  }
};


class PrototypeAST {
public:
  PrototypeAST(const std::string &name, const std::vector< ExprAST* > &args)
    : Name(name), Args(args)
  {
  }
public:
  std::string Name;
  std::vector< ExprAST* > Args;
};


class FunctionAST {
public:
  FunctionAST(PrototypeAST* proto,  std::vector< ExprAST* > body)
    : Proto(proto), Body(body)
  {
  }
public:
  std::shared_ptr< PrototypeAST >Proto;
  std::vector< ExprAST* >Body;

};
