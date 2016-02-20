#include "byte_code_gen.h"



TByteCodeGen::TByteCodeGen(const std::string fileName): Parser(new TParser(fileName)), Stack(TStack())
{
    GenByteCode();
    while (it < command.size()) {
        command[it]->UpdateStack(Stack);
        it++;
    }
    Stack.PrintResult();
}

TByteCodeGen::~TByteCodeGen() {

}

void TByteCodeGen::GenByteCode()
{
    for (size_t i = 0; i < Parser->GetRootSize(); i++) {
        Allocator(Parser->GetRootAt(i));
    }
    for (size_t i = 0; i < Parser->GetRootSize(); i++) {
        GenExprValue(Parser->GetRootAt(i), Stack.GetGlobalEnviroment());
    }
}

void TByteCodeGen::GenFuncByteCode(CallExprAST* func,  Enviroment& defineParent)
{
    for (size_t i = 0; i < func->GetArgsSize(); i++) {
        GenExprValue(func->GetArgsAt(i), defineParent);
    }
    command.push_back(std::shared_ptr<TByteCodeCMD>(new TByteCodeCMDCall(func->GetName(), func->GetArgsSize(), it, command)));
}


void TByteCodeGen::GenTaliCallByteCode(CallExprAST *func, Enviroment& defineParent, size_t pos)
{
    for (size_t i = 0; i < func->GetArgsSize(); i++) {
        GenExprValue(func->GetArgsAt(i), defineParent);
    }
    command.push_back(std::shared_ptr<TByteCodeCMD>(new TByteCodeCMDTailCall(func->GetName(), pos, func->GetArgsSize(), it)));
}

void TByteCodeGen::GenIfElseByteCode(IfElseExprAST *expr,  Enviroment& defineParent, size_t pos, std::string name)
{
    if (!name.size()) {
            GenExprValue(expr->GetTest(), defineParent);
            command.push_back(std::shared_ptr<TByteCodeCMD>(new TByteCodeCMDIfElse(it)));
            command.push_back(std::shared_ptr<TByteCodeCMD>(new TByteCodeCMDJump(it)));
            size_t k = command.size()-1;
            GenExprValue(expr->GetFirst(), defineParent);
            if (expr->IsSecond()) {
                dynamic_cast<TByteCodeCMDJump*>(command.at(k).get())->SetCurrentPos(command.size());
                command.push_back(std::shared_ptr<TByteCodeCMD>(new TByteCodeCMDJump(it)));
                k = command.size()-1;
                GenExprValue(expr->GetSecond(), defineParent);
                dynamic_cast<TByteCodeCMDJump*>(command.at(k).get())->SetCurrentPos(command.size()-1);
            } else {
                dynamic_cast<TByteCodeCMDJump*>(command.at(k).get())->SetCurrentPos(command.size());
            }
        } else {
            GenDefineByteCode(expr->GetTest(), defineParent, name, pos);
            command.push_back(std::shared_ptr<TByteCodeCMD>(new TByteCodeCMDIfElse(it)));
            command.push_back(std::shared_ptr<TByteCodeCMD>(new TByteCodeCMDJump(it)));
            size_t k = command.size()-1;
            GenDefineByteCode(expr->GetFirst(), defineParent, name, pos);
            if (expr->IsSecond()) {
                dynamic_cast<TByteCodeCMDJump*>(command.at(k).get())->SetCurrentPos(command.size());
                command.push_back(std::shared_ptr<TByteCodeCMD>(new TByteCodeCMDJump(it)));
                k = command.size()-1;
                GenDefineByteCode(expr->GetSecond(), defineParent, name, pos);
                dynamic_cast<TByteCodeCMDJump*>(command.at(k).get())->SetCurrentPos(command.size()-1);
            } else {
                dynamic_cast<TByteCodeCMDJump*>(command.at(k).get())->SetCurrentPos(command.size());
            }
    }
}

void TByteCodeGen::GenBeginByteCode(BeginExprAST *expr, Enviroment &defineParent, size_t pos, std::string name)
{
    if (!name.size()) {
        for (size_t i = 0; i < expr->GetArgsSize(); i++) {
            GenExprValue(expr->GetArgsAt(i), defineParent);
        }
    } else {
        for (size_t i = 0; i < expr->GetArgsSize(); i++) {
            GenDefineByteCode(expr->GetArgsAt(i), defineParent, name, pos);
        }
    }
    command.push_back(std::shared_ptr<TByteCodeCMD>(new TByteCodeCMDBegin(expr->GetArgsSize())));
}

void TByteCodeGen::GenLambdaByteCode(LambdaExprAST *expr, Enviroment& defineParent,  size_t pos, std::string name)
{
    std::vector<std::string> args;
    for (size_t j = 0; j < expr->GetIdentsSize(); j++) {
        args.push_back(expr->GetIdentsAt(j)->GetName());
    }
    for (size_t i = 0; i < expr->GetArgsSize(); i++) {
        GenExprValue(expr->GetArgsAt(i), defineParent);
    }
    command.push_back(std::shared_ptr<TByteCodeCMD>(new TByteCodeCMDLambda(args, defineParent)));
    if (!name.size()) {
        for (size_t i = 0; i < expr->GetBodySize(); i++) {
            GenExprValue(expr->GetBodyAt(i), defineParent);
        }
    } else {
        for (size_t i = 0; i < expr->GetBodySize(); i++) {
            GenDefineByteCode(expr->GetBodyAt(i), defineParent, name, pos);
        }
    }
}

void TByteCodeGen::GenDefineByteCode(ExprAST* expr, Enviroment& defineParent, std::string name, size_t pos)
{
    switch (expr->Type){
    case SAT_IfElse: {
        GenIfElseByteCode(dynamic_cast<IfElseExprAST*>(expr), defineParent, pos, name);
        return;
    }
    case SAT_Lambda: {
        GenLambdaByteCode(dynamic_cast<LambdaExprAST*>(expr), defineParent, pos, name);
        return;
    }
    case SAT_Define: {
        GenDefine(expr, defineParent);
        return;
    }
    case SAT_Begin: {
        GenBeginByteCode(dynamic_cast<BeginExprAST*>(expr), defineParent, pos, name);
        return;
    }
    case AT_Func: {
        if (dynamic_cast<CallExprAST*>(expr)->GetName() == name) {
            GenTaliCallByteCode(dynamic_cast<CallExprAST*>(expr), defineParent, pos);
        } else {
            GenFuncByteCode(dynamic_cast<CallExprAST*>(expr), defineParent);
        }
        return;
    }
    default:
        GenExprValue(expr, defineParent);
        return;
    }
}

void TByteCodeGen::GenDefine(ExprAST *expr, Enviroment& defineParent)
{
    FunctionAST* currentDefine = dynamic_cast<FunctionAST*>(expr);
    std::vector<std::string> args;
    for (size_t j = 0; j < currentDefine->GetProto()->GetArgsSize(); j++) {
        args.push_back(currentDefine->GetProto()->GetArgsAt(j)->GetName());
    }
    std::shared_ptr<TByteCodeCMDDefine> Define(new TByteCodeCMDDefine(currentDefine->GetProto()->GetName(), args, defineParent, command, it));
    command.push_back(Define);
    for (size_t j = 0; j < currentDefine->GetBodySize(); j++) {
        GenDefineByteCode(currentDefine->GetBodyAt(j), *(Define->GetEnviroment()), currentDefine->GetProto()->GetName(), command.size());
    }
    command.push_back(std::shared_ptr<TByteCodeCMD>(new TByteCodeCMDJumpEndDefine(it)));
}

void TByteCodeGen::Allocator(ExprAST* expr)
{
    if (IsInAllocatorValue(expr)) {
        return;
    }
    if (expr->Type != AT_Func && expr->Type != AT_Ident && expr->Type != SAT_Define && expr->Type != SAT_IfElse && expr->Type != SAT_Begin && expr->Type != SAT_Lambda && !IsInAllocatorValue(expr)){
        allocatorValue.insert(std::make_pair(expr, allocatorValue.size()));
    }
    switch (expr->Type) {
    case AT_Int: {
        command.push_back(std::shared_ptr<TByteCodeCMD>(new TByteCodeCMDAllocInt(dynamic_cast<NumberIntAST*>(expr)->GetValue())));
        return;
    }
    case AT_Double : {
        command.push_back(std::shared_ptr<TByteCodeCMD>(new TByteCodeCMDAllocDouble(dynamic_cast<NumberDoubleAST*>(expr)->GetValue())));
        return;
    }
    case AT_String : {
        command.push_back(std::shared_ptr<TByteCodeCMD>(new TByteCodeCMDAllocString(dynamic_cast<StringAST*>(expr)->GetValue())));
        return;
    }
    case AT_Symbol : {
        command.push_back(std::shared_ptr<TByteCodeCMD>(new TByteCodeCMDAllocSymbol(dynamic_cast<SymbolAST*>(expr)->GetValue())));
        return;
    }
    case AT_Char : {
        command.push_back(std::shared_ptr<TByteCodeCMD>(new TByteCodeCMDAllocChar(dynamic_cast<CharAST*>(expr)->GetValue())));
        return;
    }
    case AT_Bool : {
        command.push_back(std::shared_ptr<TByteCodeCMD>(new TByteCodeCMDAllocBool(dynamic_cast<BoolAST*>(expr)->GetValue())));
        return;
    }
    case SAT_IfElse: {
        Allocator(dynamic_cast<IfElseExprAST*>(expr)->GetTest());
        Allocator(dynamic_cast<IfElseExprAST*>(expr)->GetFirst());
        if (dynamic_cast<IfElseExprAST*>(expr)->IsSecond()) {
            Allocator(dynamic_cast<IfElseExprAST*>(expr)->GetSecond());
        }
        return;
    }
    case SAT_Begin: {
        BeginExprAST* e = dynamic_cast<BeginExprAST*>(expr);
        for (size_t i = 0; i < e->GetArgsSize(); i++) {
            Allocator(e->GetArgsAt(i));
        }
        return;
    }
    case SAT_Lambda: {
        size_t sizeArgs = dynamic_cast<LambdaExprAST*>(expr)->GetArgsSize();
        size_t sizeBody = dynamic_cast<LambdaExprAST*>(expr)->GetBodySize();
        for (size_t i = 0; i < sizeArgs; i++) {
            Allocator(dynamic_cast<LambdaExprAST*>(expr)->GetArgsAt(i));
        }
        for (size_t i = 0; i < sizeBody; i++) {
            Allocator(dynamic_cast<LambdaExprAST*>(expr)->GetBodyAt(i));
        }
        return;
    }
    case AT_Func : {
        size_t size = dynamic_cast<CallExprAST*>(expr)->GetArgsSize();
        for (size_t i = 0; i < size; i++) {
            Allocator(dynamic_cast<CallExprAST*>(expr)->GetArgsAt(i));
        }
        return;
    }
    case SAT_Define: {
        size_t size = dynamic_cast<FunctionAST*>(expr)->GetBodySize();
        for (size_t i = 0; i < size; i++) {
            Allocator(dynamic_cast<FunctionAST*>(expr)->GetBodyAt(i));
        }
        return;
    }
    case AT_Ident: {
        return;
    }
    default: {
        throw new ParserExceptionIncorrectToken("cannot identify token type");
        return;
    }
    }
}

void TByteCodeGen::GenExprValue(ExprAST *expr, Enviroment& defineParent) {
    switch (expr->Type) {
    case AT_Ident : {
        command.push_back(std::shared_ptr<TByteCodeCMD>(new TByteCodeCMDPushIdent(dynamic_cast<IdentAST*>(expr)->GetName())));
        return;
    }
    case SAT_IfElse: {
        GenIfElseByteCode(dynamic_cast<IfElseExprAST*>(expr), defineParent);
        return;
    }
    case AT_Func : {
        GenFuncByteCode(dynamic_cast<CallExprAST*>(expr), defineParent);
        return;
    }
    case SAT_Begin: {
        GenBeginByteCode(dynamic_cast<BeginExprAST*>(expr), defineParent);
        return;
    }
    case SAT_Define: {
        GenDefine(expr, defineParent);
        return;
    }
    case SAT_Lambda: {
        GenLambdaByteCode(dynamic_cast<LambdaExprAST*>(expr), defineParent);
        return;
    }
    default: {
        command.push_back(std::shared_ptr<TByteCodeCMD>(new TByteCodeCMDPush(GetAllocatorValue(expr))));
        return;
    }
    }
}

size_t TByteCodeGen::GetAllocatorValue(ExprAST *expr)
{
    for (auto it = allocatorValue.begin(); it != allocatorValue.end(); it++) {
        if (expr->Type == it->first->Type) {
            switch (expr->Type) {
            case AT_Int: {
                if (dynamic_cast<NumberIntAST*>(expr)->GetValue() == dynamic_cast<NumberIntAST*>(it->first)->GetValue()) {
                    return it->second;
                }
                break;
            }
            case AT_Double: {
                if (dynamic_cast<NumberDoubleAST*>(expr)->GetValue() == dynamic_cast<NumberDoubleAST*>(it->first)->GetValue()) {
                    return it->second;
                }
                break;
            }
            case AT_Char: {
                if (dynamic_cast<CharAST*>(expr)->GetValue() == dynamic_cast<CharAST*>(it->first)->GetValue()) {
                    return it->second;
                }
                break;
            }
            case AT_String: {
                if (dynamic_cast<StringAST*>(expr)->GetValue() == dynamic_cast<StringAST*>(it->first)->GetValue()) {
                    return it->second;
                }
                break;
            }
            case AT_Bool: {
                if (dynamic_cast<BoolAST*>(expr)->GetValue() == dynamic_cast<BoolAST*>(it->first)->GetValue()) {
                    return it->second;
                }
                break;
            }
            case AT_Symbol: {
                if (dynamic_cast<SymbolAST*>(expr)->GetValue() == dynamic_cast<SymbolAST*>(it->first)->GetValue()) {
                    return it->second;
                }
                break;
            }
            default: {
                break;
            }
            }
        }
    }
}

bool TByteCodeGen::IsInAllocatorValue(ExprAST *expr)
{
    for (auto it = allocatorValue.begin(); it != allocatorValue.end(); it++) {
        if (expr->Type == it->first->Type) {
            switch (expr->Type) {
            case AT_Int: {
                if (dynamic_cast<NumberIntAST*>(expr)->GetValue() == dynamic_cast<NumberIntAST*>(it->first)->GetValue()) {
                    return true;
                }
                break;
            }
            case AT_Double: {
                if (dynamic_cast<NumberDoubleAST*>(expr)->GetValue() == dynamic_cast<NumberDoubleAST*>(it->first)->GetValue()) {
                    return true;
                }
                break;
            }
            case AT_Char: {
                if (dynamic_cast<CharAST*>(expr)->GetValue() == dynamic_cast<CharAST*>(it->first)->GetValue()) {
                    return true;
                }
                break;
            }
            case AT_String: {
                if (dynamic_cast<StringAST*>(expr)->GetValue() == dynamic_cast<StringAST*>(it->first)->GetValue()) {
                    return true;
                }
                break;
            }
            case AT_Bool: {
                if (dynamic_cast<BoolAST*>(expr)->GetValue() == dynamic_cast<BoolAST*>(it->first)->GetValue()) {
                    return true;
                }
                break;
            }
            case AT_Symbol: {
                if (dynamic_cast<SymbolAST*>(expr)->GetValue() == dynamic_cast<SymbolAST*>(it->first)->GetValue()) {
                    return true;
                }
                break;
            }
            default: {
                break;
            }
            }
        }
    }
    return false;
}
