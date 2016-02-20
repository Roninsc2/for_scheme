#include "parser.h"


TParser::TParser(const std::string fileName): Lexer(new TLexer(fileName))
{
    stdFunc.insert(std::make_pair("define", &TParser::ParseDefineFunc));
    stdFunc.insert(std::make_pair("begin", &TParser::ParseBeginFunc));
    stdFunc.insert(std::make_pair("lambda", &TParser::ParseLambdaFunc));
    stdFunc.insert(std::make_pair("if", &TParser::ParseIfElseFunc));
    stdFunc.insert(std::make_pair("cond", &TParser::ParseCondFunc));
    Parse();
}

void TParser::Parse() {

    while(1) {
        GetNextToken();
        switch (CurrentToken.GetState()) {
        case State_EOF : return;
        case State_Ident : {
            if (stdFunc.count(CurrentToken.GetValue())) {
                root.push_back(std::shared_ptr<ExprAST>((this->*(stdFunc.at(CurrentToken.GetValue())))()));
            } else {
                root.push_back(std::shared_ptr<ExprAST>(ParseCallExprAST()));
            }
            break;
        }
        case State_Lbkt: {
            continue;
            break;
        }
        case State_Rbkt: {
            throw new ParserExceptionIncorrectToken("incorrect token: ) ");
            break;
        }
        default:
            root.push_back(std::shared_ptr<ExprAST>(GetExprType()));
        }
    }
}

void TParser::GetNextToken() {
    CurrentToken = Lexer->GetTokenAt(i);
    i++;
}

ExprAST *TParser::GetRootAt(size_t i)
{
    return (i < root.size() ? root.at(i).get() : nullptr);
}

size_t TParser::GetRootSize()
{
    return root.size();
}

ExprAST* TParser::ParseCallExprAST()
{
    std::string Name = CurrentToken.GetValue();
    if (stdFunc.count(CurrentToken.GetValue())) {
        return (this->*(stdFunc.at(CurrentToken.GetValue())))();
    }
    std::vector<std::shared_ptr<ExprAST> > Args;

    while(1) {
        GetNextToken();
        if (CurrentToken.GetState() == State_Rbkt) {
            break;
        }
        Args.push_back(std::shared_ptr<ExprAST>(GetExprType()));
    } ;
    return (new CallExprAST(Name, Args));
}

ExprAST* TParser::GetExprType() {
    switch (CurrentToken.GetState()) {
    case State_Lbkt : {
        GetNextToken();
        return (ParseCallExprAST());
    }
    case State_Ident: {
        return (new IdentAST(CurrentToken.GetValue()));
    }
    case State_Bool: {
        if (CurrentToken.GetValue() == "#t")  {
            return (new BoolAST(true));
        } else {
            return (new BoolAST(false));
        }
    }
    case State_String : {
        std::string val = CurrentToken.GetValue();
        val.erase(0,1);
        val.erase(val.size()-1, val.size());
        return (new StringAST(val));
    }
    case State_Symbol : {

        std::string val = CurrentToken.GetValue();
        if (val.length() != 1) {
            val.erase(0,1);
            return (new SymbolAST(val));
        } else {
            GetNextToken();
            if (CurrentToken.GetState() == State_Lbkt) {
                return ParseList();
            }
        }
    }
    case State_Char : {
        char val  = CurrentToken.GetValue()[2];
        return (new CharAST(val));
    }
    case State_Int : {
        int val  = std::stoi(CurrentToken.GetValue());
        return (new NumberIntAST(val));
    }
    case State_Double : {
        double val  =  std::stod(CurrentToken.GetValue());
        return (new NumberDoubleAST(val));
    }
    default:
        throw new ParserExceptionIncorrectToken("cannot identify token type");
        break;
    }
}

CallExprAST* TParser::ParseList() {
    GetNextToken();
    std::vector<std::shared_ptr<ExprAST> > expr;
    if (CurrentToken.GetState() == State_Rbkt) {
        return (new CallExprAST("list", expr));
    } else {
        expr.push_back(std::shared_ptr<ExprAST>(GetExprTypeForList()));
        while (1) {
            GetNextToken();
            if (CurrentToken.GetState() == State_Rbkt) {
                break;
            } else if (CurrentToken.GetState() == State_Point && expr.size()) {
                if (IsCorrectPair()) {
                    return ConvertToPairAndList(expr);
                } else {
                    throw new ParserExceptionIncorrectToken("incorrect pair");
                }

            } else {
                expr.push_back(std::shared_ptr<ExprAST>(GetExprTypeForList()));
            }
        }
        return (new CallExprAST("list", expr));
    }
}

ExprAST* TParser::GetExprTypeForList() {
    switch (CurrentToken.GetState()) {
    case State_Lbkt : {
        return ParseList();
    }
    case State_Ident:
        return (new SymbolAST(CurrentToken.GetValue()));
    case State_Bool: {
        if (CurrentToken.GetValue() == "#t")  {
            return (new BoolAST(true));
        } else {
            return (new BoolAST(false));
        }
    }
    case State_String : {
        std::string val = CurrentToken.GetValue();
        val.erase(0,1);
        val.erase(val.size()-1, val.size());
        return (new StringAST(val));
    }
    case State_Symbol : {

        std::string val = CurrentToken.GetValue();
        if (val.length() != 1) {
            val.erase(0,1);
            return (new SymbolAST(val));
        } else {
            GetNextToken();
            if (CurrentToken.GetState() == State_Lbkt) {
                return ParseList();
            }
        }
    }
    case State_Char : {
        char val  = CurrentToken.GetValue()[2];
        return (new CharAST(val));
    }
    case State_Int : {
        int val  = std::stoi(CurrentToken.GetValue());
        return (new NumberIntAST(val));
    }
    case State_Double : {
        double val  =  std::stod(CurrentToken.GetValue());
        return (new NumberDoubleAST(val));
    }
    default: {
        throw new ParserExceptionIncorrectToken("cannot identify token type");
        break;
    }
    }
}

ExprAST* TParser::ParseDefineFunc()
{
    GetNextToken();
    if (CurrentToken.GetState() != State_Lbkt) {
        GetNextToken();
        ExprAST* ident = GetExprType();
        GetNextToken();
        ExprAST* value = GetExprType();
        GetNextToken();
        if (ident->Type != AT_Ident && (value->Type == SAT_Define || value->Type == SAT_Lambda) && CurrentToken.GetState() != State_Rbkt) {
            throw new ParserExceptionIncorrectToken("error in define");
        } else {
            std::vector<std::shared_ptr<IdentAST> > args;
            std::vector<std::shared_ptr<ExprAST>> body;
            body.push_back(std::shared_ptr<ExprAST>(value));
            return new FunctionAST(new PrototypeAST(dynamic_cast<IdentAST*>(ident)->GetName(), args), body);
        }
    }
    GetNextToken();
    std::string name;
    std::vector<std::shared_ptr<IdentAST> > args;
    if (CurrentToken.GetState() != State_Ident) {
        throw new ParserExceptionIncorrectToken(CurrentToken.GetValue());
    } else {
        name = CurrentToken.GetValue();
    }
    while(true) {
        GetNextToken();
        if (CurrentToken.GetState() == State_Rbkt) {
            break;
        }
        if (CurrentToken.GetState() == State_Ident) {
            args.push_back(std::shared_ptr<IdentAST>(new IdentAST(CurrentToken.GetValue())));
        } else {
            throw new ParserExceptionIncorrectToken(CurrentToken.GetValue());
        }
    }
    PrototypeAST* proto = new PrototypeAST(name, args);
    std::vector<std::shared_ptr<ExprAST>> body;
    while(true) {
        GetNextToken();
        if (CurrentToken.GetState() == State_Rbkt) {
            break;
        }
        body.push_back(std::shared_ptr<ExprAST>(GetExprType()));
    }
    return (new FunctionAST(proto, body));
}

ExprAST* TParser::ParseIfElseFunc()
{
    GetNextToken();
    if (CurrentToken.GetState() == State_Rbkt) {
        throw new ParserExceptionIncorrectToken("if incoorect");
    }
    ExprAST* test = GetExprType();
    GetNextToken();
    if (CurrentToken.GetState() == State_Rbkt) {
        throw new ParserExceptionIncorrectToken("if incoorect");
    }
    ExprAST* first = GetExprType();
    GetNextToken();
    if (CurrentToken.GetState() == State_Rbkt) {
        return (new IfElseExprAST(test, first, nullptr));
    } else {
        ExprAST* second = GetExprType();
        GetNextToken();
        if (CurrentToken.GetState() != State_Rbkt) {
            throw new ParserExceptionIncorrectToken("if incoorect");
        }
        return new IfElseExprAST(test, first, second);
    }
}

ExprAST* TParser::ParseLambdaFunc()
{
    GetNextToken();
    if (CurrentToken.GetState() != State_Lbkt) {
        throw new ParserExceptionIncorrectToken("lambda incoorect at parser 262");
    }
    std::vector<std::shared_ptr<IdentAST> > Idents;
    while (true) {
        GetNextToken();
        if (CurrentToken.GetState() == State_Rbkt) {
            break;
        }
        if (CurrentToken.GetState() == State_Ident) {
            Idents.push_back(std::shared_ptr<IdentAST>(new IdentAST(CurrentToken.GetValue())));
        } else {
            throw new ParserExceptionIncorrectToken("lambda incoorect at parser 273");
        }
    }
    std::vector<std::shared_ptr<ExprAST>> body;
    while (true) {
        GetNextToken();
        if (CurrentToken.GetState() == State_Rbkt) {
            break;
        }
        body.push_back(std::shared_ptr<ExprAST>(GetExprType()));
    }
    std::vector<std::shared_ptr<ExprAST>> args;
    if (Lexer->GetTokenAt(i+Idents.size()).GetState() == State_Rbkt) {
        while(true) {
            GetNextToken();
            if (CurrentToken.GetState() == State_Rbkt) {
                break;
            }
            args.push_back(std::shared_ptr<ExprAST>(GetExprType()));
        }
    }
    return new LambdaExprAST(Idents, args, body);
}

ExprAST* TParser::ParseCondFunc()
{

}

ExprAST* TParser::ParseBeginFunc()
{
    std::vector<std::shared_ptr<ExprAST>> args;
    while (true) {
        GetNextToken();
        if (CurrentToken.GetState() == State_Rbkt) {
            break;
        }
        args.push_back(std::shared_ptr<ExprAST>(GetExprType()));
    }
    return new BeginExprAST(args);
}

CallExprAST* TParser::ConvertToPairAndList(std::vector<std::shared_ptr<ExprAST> > expr) {
    std::shared_ptr<ExprAST> firstInCons = expr[expr.size()-1];
    expr.pop_back();
    CallExprAST* list = new CallExprAST("list", expr);
    std::vector<std::shared_ptr<ExprAST> > forCons;
    forCons.push_back(firstInCons);
    GetNextToken();
    forCons.push_back(std::shared_ptr<ExprAST>(GetExprTypeForList()));
    CallExprAST* cons = new CallExprAST("cons", forCons);
    std::vector<std::shared_ptr<ExprAST> > forAppend;
    forAppend.push_back(std::shared_ptr<CallExprAST>(list));
    forAppend.push_back(std::shared_ptr<CallExprAST>(cons));
    CallExprAST* append = new CallExprAST("append", forAppend);
    return append;
}

bool TParser::IsCorrectPair() {
    GetNextToken();
    if (CurrentToken.GetState() == State_Rbkt) {
        i--;
        return false;
    }
    GetNextToken();
    if (CurrentToken.GetState() == State_Rbkt) {
        i -= 2;
        return true;
    } else {
        i -= 2;
        return false;
    }
}
