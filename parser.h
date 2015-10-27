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
    VList* ParseList();
    VList* GetListType();

private:
    std::shared_ptr< TLexer > Lexer;
    Token CurrentToken;
    int i = 0;
    std::vector< ExprAST* > root;
};

