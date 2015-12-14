#pragma once

#include "parser.h"
#include <unordered_map>
#include <saveload.h>


/* command
 * 0 - allocator
 * 1 - push to stack
 * 2 - push ident
 * 3 - call
 * 4 - TAil call
 * 5 - func end
 * 6 - define start
 * 7 - define end
 * 8 - if else
    */

class TByteCode {
public:
    TByteCode(const std::string fileName, const std::string outputFile);
    ~TByteCode();
    void GetByteCode();
    void GetFuncByteCode(CallExprAST* func);
    void GetIfElseByteCode(IfElseExprAST* expr, std::string name = "");
    void GetTaliCallByteCode(CallExprAST* func);
    void GetDefineByteCode(ExprAST* expr, std::string name);
    void Allocator(ExprAST* value);
    void GetExprValue(ExprAST* expr);
    size_t GetAllocatorValue(ExprAST* expr);
    bool IsInAllocatorValue(ExprAST* epxr);
private:
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
    std::shared_ptr< TParser > Parser;
    std::unordered_map<ExprAST*, size_t> allocatorValue;
    std::ofstream fout;
};
