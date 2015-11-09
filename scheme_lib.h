#pragma once
#include "ast.h"

TPairType* GetPairType(ExprAST* expr);



VList* list(std::vector< ExprAST* > exprs);

void plus(std::vector<ExprAST*> expr);



