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
    AT_Ident,//6
    AT_Func,//7
    SAT_IfElse,
    SAT_Cond,
    SAT_Begin,
    SAT_Lambda,
    SAT_Define
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
  std::vector<std::shared_ptr<ExprAST> > Args;

public:
  CallExprAST(const std::string &callee, std::vector<std::shared_ptr<ExprAST> > &args)
    : Callee(callee), Args(args)
  {
      Type = AT_Func;
  }
};


class IfElseExprAST : public ExprAST {
public:
    std::shared_ptr<ExprAST> test;
    std::shared_ptr<ExprAST> first;
    std::shared_ptr<ExprAST> second;

public:
  IfElseExprAST(ExprAST* TEST, ExprAST* First, ExprAST* Second)
    : test(TEST),first(First), second(Second)
  {
      Type = SAT_IfElse;
  }
};

class CondExprAST : public ExprAST {
public:
    std::vector<std::shared_ptr<ExprAST> > Tests;
    std::vector<std::shared_ptr<ExprAST> > Args;

public:
  CondExprAST(std::vector<std::shared_ptr<ExprAST> > expr, std::vector<std::shared_ptr<ExprAST> > test)
    : Tests(test), Args(expr)
  {
      Type = SAT_Cond;
  }
};

class BeginExprAST : public ExprAST {
public:
    std::vector<std::shared_ptr<ExprAST> > Args;

public:
  BeginExprAST(std::vector<std::shared_ptr<ExprAST> > exprs)
    : Args(exprs)
  {
      Type = SAT_Begin;
  }
};

class LambdaExprAST : public ExprAST {
public:
    std::vector<std::shared_ptr<IdentAST> > Idents;
    std::vector<std::shared_ptr<ExprAST> > Args;
    std::vector<std::shared_ptr<ExprAST> >Body;

public:
  LambdaExprAST(std::vector<std::shared_ptr<IdentAST> > idents, std::vector<std::shared_ptr<ExprAST> > args, std::vector<std::shared_ptr<ExprAST> > body)
    : Idents(idents), Args(args), Body(body)
  {
      Type = SAT_Lambda;
  }
};


class PrototypeAST {
public:
  PrototypeAST(const std::string &name, const std::vector<std::shared_ptr<IdentAST> > &args)
    : Name(name), Args(args)
  {
  }
public:
  std::string Name;
  std::vector<std::shared_ptr<IdentAST> > Args;
};


class FunctionAST: public ExprAST {
public:
  FunctionAST(PrototypeAST* proto,  std::vector<std::shared_ptr<ExprAST> > body)
    : Proto(proto), Body(body)
  {
      Type = SAT_Define;
  }
public:
  std::shared_ptr< PrototypeAST >Proto;
  std::vector<std::shared_ptr<ExprAST> >Body;

};
