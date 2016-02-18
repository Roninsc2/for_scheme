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
    std::shared_ptr<ExprType> StackPop();
    void StackPushBack(std::shared_ptr<ExprType> expr);
    size_t GetStackSize();
    void AllocatorPushBack(std::shared_ptr<ExprType> expr);
    std::shared_ptr<ExprType> GetAllocatorAt(size_t i);
    Enviroment& GetGlobalEnviroment();
    Enviroment* GetCurrentEnviroment();
    void ResetCurrentEnviroment(std::shared_ptr<Enviroment>& env);
private:
    std::vector< std::shared_ptr<ExprType> > stack;
    std::vector< std::shared_ptr<ExprType> > allocator;
    Enviroment define;
    std::shared_ptr<Enviroment> currentDefine;
};
