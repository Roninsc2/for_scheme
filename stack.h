#pragma once
#include <fstream>
#include <string>
#include <cctype>
#include <vector>
#include <map>
#include <memory>
#include "type.h"

class TStack {
public:
    TStack();
    ~TStack();
    void PrintResult();
public:
    std::vector< std::shared_ptr<ExprType> > stack;
    std::vector< std::shared_ptr<ExprType> > allocator;
    std::map<std::string, std::shared_ptr<IdentType> > defineVar;
    std::map<std::string, std::shared_ptr<FunctionType> > defineFunc;
};
