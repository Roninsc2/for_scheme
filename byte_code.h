#pragma once

#include "parser.h"


class TByteCode {
public:
    TByteCode(const std::string fileName, const std::string outputFile);
    ~TByteCode();
    void GetByteCode();
    void GetFuncByteCode(CallExprAST* func, size_t level);
    void GetExprValue(ExprAST* expr, size_t level);
private:
    std::shared_ptr< TParser > Parser;
    std::ofstream fout;
};
