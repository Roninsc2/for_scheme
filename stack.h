#pragma once
#include <fstream>
#include <string>
#include <cctype>
#include <vector>
#include <map>
#include "byte_code_gen.h"
#include "byte_code_cmd.h"

class TStack {
public:
    TStack(const std::string fileName);
    ~TStack();
private:
    void DoCode();
    void PrintResult();
private:
    std::vector< ExprType* > stack;
    std::vector< ExprType* > allocator;
    std::map<std::string, IdentType* > defineVar;
    std::map<std::string, FunctionType* > defineFunc;
    std::shared_ptr<TByteCodeGen> byteCodeGen;
    std::shared_ptr <TByteCodeCMD> byteCodeCMD;
    size_t it = 0;
};
