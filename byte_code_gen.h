#pragma once
#include "parser.h"
#include <map>
#include <sstream>
#include "byte_code_cmd.h"
#include "stack.h"


class TByteCodeGen {
public:
    TByteCodeGen(const std::string fileName);
    ~TByteCodeGen();
    void GenByteCode();
    void GenFuncByteCode(CallExprAST* func);
    void GenIfElseByteCode(IfElseExprAST* expr, size_t pos = 0,  std::string name = "");
    void GenTaliCallByteCode(CallExprAST* func, size_t pos);
    void GenDefineByteCode(ExprAST* expr, std::string name, size_t pos);
    void Allocator(ExprAST* value);
    void GenExprValue(ExprAST* expr);
    size_t GenAllocatorValue(ExprAST* expr);
    bool IsInAllocatorValue(ExprAST* epxr);
private:
    std::shared_ptr< TParser > Parser;
    std::map<ExprAST*, size_t> allocatorValue;
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
    std::vector<std::shared_ptr<TByteCodeCMD>> command;
    TStack Stack;
    size_t it = 0;
};
