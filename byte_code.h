#pragma once

#include "parser.h"
#include "scheme_lib.h"


class TByteCode {
public:
    TByteCode(const std::string fileName, const std::string outputFile);
    ~TByteCode();
    void GetByteCode();
    void GetFuncByteCode(CallExprAST* func);
    void GetExprValue(ExprAST* expr);
private:
    std::shared_ptr< TParser > Parser;
    std::ofstream fout;
};
