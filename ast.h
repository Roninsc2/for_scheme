#pragma once
#include <string>
#include <vector>
#include <memory>
#include <vlist.h>

class ExprAST {
public:
    virtual ~ExprAST(){}
};

class NumberIntAST : public ExprAST {

public:
    NumberIntAST(int val) : value(val) {}

public:
    int value;
};

class NumberDoubleAST : public ExprAST {

public:
    NumberDoubleAST(double val) : value(val) {}

public:
    double value;
};


class IdentAST : public ExprAST {

public:
    IdentAST(std::string val) : value(val) {}

public:
    std::string value;
};

class StringAST : public ExprAST {

public:
    StringAST(std::string val) : value(val) {}

public:
    std::string value;
};

class CharAST : public ExprAST {

public:
    CharAST(char val) : value(val) {}

public:
    char value;
};

class BoolAST : public ExprAST {

public:
    BoolAST(bool val) : value(val) {}

public:
    bool value;
};

class SymbolAST : public ExprAST {

public:
    SymbolAST(std::string val) : value(val) {}

public:
    std::string value;
};

class ListAST : public ExprAST {

public:
    ListAST(VList* val) : value(val) {}

public:
    std::shared_ptr<VList> value;
};


class CallExprAST : public ExprAST {
  std::string Callee;
  std::vector< ExprAST* > Args;

public:
  CallExprAST(const std::string &callee, std::vector< ExprAST* > &args)
    : Callee(callee), Args(args) {}
};


class PrototypeAST {
  std::string Name;
  std::vector< std::shared_ptr<ExprAST> > Args;

public:
  PrototypeAST(const std::string &name, const std::vector<std::shared_ptr<ExprAST> > &args)
    : Name(name), Args(args) {}
};


class FunctionAST {
  std::shared_ptr< PrototypeAST >Proto;
  std::vector< std::shared_ptr<ExprAST> >Body;

public:
  FunctionAST(std::shared_ptr< PrototypeAST > proto,  std::vector< std::shared_ptr<ExprAST> > body)
    : Proto(proto), Body(body) {}
};
