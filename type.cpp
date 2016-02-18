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

std::shared_ptr< PrototypeType >& FunctionType::GetPrototype() {
    return Proto;
}

std::shared_ptr<Enviroment>& FunctionType::GetEnviroment() {
    return Env;
}


std::string &PrototypeType::GetArgsAt(size_t i) {
    return Args.at(i);
}

size_t PrototypeType::GetArgsSize() {
    return Args.size();
}
