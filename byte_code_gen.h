#pragma once

#include "parser.h"
#include <unordered_map>
#include <sstream>


class TByteCodeGen {
public:
    TByteCodeGen(const std::string fileName);
    ~TByteCodeGen();
    void GeTByteCodeGen();
    void GetFuncByteCode(CallExprAST* func);
    void GetIfElseByteCode(IfElseExprAST* expr, std::stringstream::pos_type pos = 0,  std::string name = "");
    void GetTaliCallByteCode(CallExprAST* func, std::stringstream::pos_type pos);
    void GetDefineByteCode(ExprAST* expr, std::string name, std::stringstream::pos_type pos);
    void Allocator(ExprAST* value);
    void GetExprValue(ExprAST* expr);
    size_t GetAllocatorValue(ExprAST* expr);
    bool IsInAllocatorValue(ExprAST* epxr);
private:
    std::shared_ptr< TParser > Parser;
    std::unordered_map<ExprAST*, size_t> allocatorValue;
public:
    enum Command {
        CMD_AllOC = '0',
        CMD_PUSH = '1',
        CMD_PUSHIDENT = '2',
        CMD_IFELSE = '3',
        CMD_CALL = '4',
        CMD_TAILCALL = '5',
        CMD_DEFSTART = '6',
        CMD_ENDCALL = '7',
        CMD_ENDDEF = '8',
    };

    enum ValueType {
        VT_INT = '0',
        VT_DOUBLE = '1',
        VT_STRING = '2',
        VT_SYMBOL = '3',
        VT_CHAR = '4',
        VT_BOOL = '5'
    };
    std::stringstream bytecodeString;
};
