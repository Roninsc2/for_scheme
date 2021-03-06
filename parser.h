#pragma once
#include <memory>
#include "lexer.h"
#include "ast.h"
#include <map>

class TParser {

public:
    TParser(const std::string fileName);
    ~TParser(){
    }
    void GetNextToken();
    ExprAST* GetRootAt(size_t i);
    size_t GetRootSize();

private:
    ExprAST* ParseCallExprAST();
    ExprAST* GetExprType();
    CallExprAST* ParseList();
    ExprAST* GetExprTypeForList();
    ExprAST* ParseDefineFunc();
    ExprAST* ParseIfElseFunc();
    ExprAST* ParseLambdaFunc();
    ExprAST* ParseCondFunc();
    ExprAST* ParseBeginFunc();
    CallExprAST* ConvertToPairAndList(std::vector< std::shared_ptr<ExprAST> > expr);
    bool IsCorrectPair();
    void Parse();

private:
    std::shared_ptr< TLexer > Lexer;
    std::map<std::string, ExprAST*(TParser::*)()> stdFunc;
    Token CurrentToken;
    int i = 0;
    std::vector< std::shared_ptr<ExprAST> > root;
};
