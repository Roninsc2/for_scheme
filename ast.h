#pragma once

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

