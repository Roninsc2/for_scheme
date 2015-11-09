#pragma once
#include <memory>
#include "lexer.h"
#include "ast.h"

class TParser {

public:
    TParser(const std::string fileName);
    ~TParser(){}
    void GetNextToken();

private:
    ExprAST* ParseCallExprAST();
    ExprAST* GetExprType();
    CallExprAST* ParseList();
    ExprAST* GetExprTypeForList();
    bool IsCorrectPair();
    void Parse();

private:
    std::shared_ptr< TLexer > Lexer;
    Token CurrentToken;
    int i = 0;
public:
    std::vector< ExprAST* > root;
};

