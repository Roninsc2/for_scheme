#include "type.h"

size_t FunctionType::GetStart() {
    return Start;
}

void FunctionType::ResetStart(size_t i) {
    Start = i;
}

size_t FunctionType::GetEnd() {
    return End;
}

void FunctionType::ResetEnd(size_t i) {
    End = i;
}

PrototypeType* FunctionType::GetPrototype() {
    return Proto.get();
}

Enviroment* FunctionType::GetEnviroment() {
    return Env.get();
}


std::string &PrototypeType::GetArgsAt(size_t i) {
    return Args.at(i);
}

size_t PrototypeType::GetArgsSize() {
    return Args.size();
}

int NumberIntType::GetValue() {
    return value;
}

double NumberDoubleType::GetValue() {
    return value;
}

std::shared_ptr<ExprType> &IdentType::GetValue() {
    return value;
}

void IdentType::ResetValue(std::shared_ptr<ExprType> &expr) {
    value.reset();
    value = expr;
}

std::string StringType::GetValue() {
    return value;
}

char CharType::GetValue() {
    return value;
}

bool BoolType::IsTrue() {
    return value;
}

bool BoolType::GetValue() {
    return value;
}

std::string SymbolType::GetValue() {
    return value;
}

VList *ListType::GetValue() {
    return value.get();
}

std::map<std::string, std::shared_ptr<ExprType> > Enviroment::GetEnviroment() {
    return This;
}

Enviroment *Enviroment::GetParent() {
    return Parent;
}

void Enviroment::ResetParent(Enviroment *env) {
    Parent = env;
}

void Enviroment::InsertToEnviroment(std::string name, std::shared_ptr<ExprType> expr) {
    This.insert(std::make_pair(name, expr));
}

ExprType *Enviroment::Find(std::string name) {
    return This.at(name).get();
}

Enviroment *Enviroment::FindVarible(std::string name) {
    Enviroment* env = this;
    while (env->Parent) {
        if (env->This.count(name)) {
            return env;
        }
        env = env->Parent;
    }
    if (env->This.count(name)) {
        return env;
    }
    return nullptr;
}
