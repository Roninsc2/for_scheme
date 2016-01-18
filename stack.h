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
    std::vector< std::shared_ptr<ExprType> > stack;
    std::vector< std::shared_ptr<ExprType> > allocator;
    std::map<std::string, std::shared_ptr<IdentType> > defineVar;
    std::map<std::string, std::shared_ptr<FunctionType> > defineFunc;
    std::shared_ptr<TByteCodeGen> byteCodeGen;
    size_t it = 0;
};
