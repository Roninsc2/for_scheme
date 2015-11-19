#include "parser.h"


TParser::TParser(const std::string fileName): Lexer(new TLexer(fileName))
{
    Parse();
}

void TParser::Parse() {

    while(1) {
        GetNextToken();
        switch (CurrentToken.state) {
        case State_EOF : return;
        case State_Ident : {
            if (CurrentToken.value == "define" && Lexer->Tokens[i].state == State_Lbkt) {
                GetNextToken();
                define.push_back(ParseDefineFunc());
            } else {
                root.push_back(ParseCallExprAST());
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
            root.push_back(GetExprType());
        }
    }
}

void TParser::GetNextToken() {
    CurrentToken = Lexer->Tokens[i];
    i++;
}

ExprAST* TParser::ParseCallExprAST()
{
    std::string Name = CurrentToken.value;
    std::vector< ExprAST* > Args;

    while(1) {
        GetNextToken();
        if (CurrentToken.state == State_Rbkt) {
            break;
        }
        Args.push_back(GetExprType());
    } ;
    return (new CallExprAST(Name, Args));
}

ExprAST* TParser::GetExprType() {
    switch (CurrentToken.state) {
    case State_Lbkt : {
        GetNextToken();
        return (ParseCallExprAST());
    }
    case State_Ident: {
        return (new IdentAST(CurrentToken.value));
    }
    case State_Bool: {
        if (CurrentToken.value == "#t")  {
            return (new BoolAST(true));
        } else {
            return (new BoolAST(false));
        }
    }
    case State_String : {
        std::string val = CurrentToken.value;
        val.erase(0,1);
        val.erase(val.size()-1, val.size());
        return (new StringAST(val));
    }
    case State_Symbol : {

        std::string val = CurrentToken.value;
        if (val.length() != 1) {
            val.erase(0,1);
            return (new SymbolAST(val));
        } else {
            GetNextToken();
            if (CurrentToken.state == State_Lbkt) {
                return ParseList();
            }
        }
    }
    case State_Char : {
        char val  = CurrentToken.value[2];
        return (new CharAST(val));
    }
    case State_Int : {
        int val  = std::stoi(CurrentToken.value);
        return (new NumberIntAST(val));
    }
    case State_Double : {
        double val  =  std::stod(CurrentToken.value);
        return (new NumberDoubleAST(val));
    }
    default:
        throw new ParserExceptionIncorrectToken("cannot identify token type");
        break;//error
    }
}

CallExprAST* TParser::ParseList() {
    GetNextToken();
    std::vector< ExprAST* > expr;
    if (CurrentToken.state == State_Rbkt) {
        return (new CallExprAST("list", expr));
    } else {
        expr.push_back(GetExprTypeForList());
        while (1) {
            GetNextToken();
            if (CurrentToken.state == State_Rbkt) {
                break;
            } else if (CurrentToken.state == State_Point && expr.size()) {
                if (IsCorrectPair()) {
                    return ConvertToPairAndList(expr);
                } else {
                    throw new ParserExceptionIncorrectToken("incorrect pair");
                }

            } else {
                expr.push_back(GetExprTypeForList());
            }
        }
        return (new CallExprAST("list", expr));
    }
}

ExprAST* TParser::GetExprTypeForList() {
    switch (CurrentToken.state) {
    case State_Lbkt : {
        return ParseList();
    }
    case State_Ident:
        return (new SymbolAST(CurrentToken.value));
    case State_Bool: {
        if (CurrentToken.value == "#t")  {
            return (new BoolAST(true));
        } else {
            return (new BoolAST(false));
        }
    }
    case State_String : {
        std::string val = CurrentToken.value;
        val.erase(0,1);
        val.erase(val.size()-1, val.size());
        return (new StringAST(val));
    }
    case State_Symbol : {

        std::string val = CurrentToken.value;
        if (val.length() != 1) {
            val.erase(0,1);
            return (new SymbolAST(val));
        } else {
            GetNextToken();
            if (CurrentToken.state == State_Lbkt) {
                return ParseList();
            }
        }
    }
    case State_Char : {
        char val  = CurrentToken.value[2];
        return (new CharAST(val));
    }
    case State_Int : {
        int val  = std::stoi(CurrentToken.value);
        return (new NumberIntAST(val));
    }
    case State_Double : {
        double val  =  std::stod(CurrentToken.value);
        return (new NumberDoubleAST(val));
    }
    default: {
        throw new ParserExceptionIncorrectToken("cannot identify token type");
        break;
    }
    }
}

FunctionAST* TParser::ParseDefineFunc()
{
    GetNextToken();
    std::string name;
    std::vector< ExprAST* > args;
    if (CurrentToken.state != State_Ident) {
        //error
    } else {
        name = CurrentToken.value;
    }
    while(true) {
        GetNextToken();
        if (CurrentToken.state == State_Rbkt) {
            break;
        }
        if (CurrentToken.state == State_Ident) {
            args.push_back(new IdentAST(CurrentToken.value));
        } else {
            //error
        }
    }
    PrototypeAST* proto = new PrototypeAST(name, args);
    std::vector<ExprAST*> body;
    while(true) {
        GetNextToken();
        if (CurrentToken.state == State_Rbkt) {
            break;
        }
        body.push_back(GetExprType());
    }
    return (new FunctionAST(proto, body));
}

CallExprAST* TParser::ConvertToPairAndList(std::vector< ExprAST *> expr) {
    ExprAST* firstInCons = expr[expr.size()-1];
    expr.pop_back();
    CallExprAST* list = new CallExprAST("list", expr);
    std::vector <ExprAST*> forCons;
    forCons.push_back(firstInCons);
    GetNextToken();
    forCons.push_back(GetExprTypeForList());
    CallExprAST* cons = new CallExprAST("cons", forCons);
    std::vector<ExprAST*> forAppend;
    forAppend.push_back(list);
    forAppend.push_back(cons);
    CallExprAST* append = new CallExprAST("append", forAppend);
    return append;
}

bool TParser::IsCorrectPair() {
    GetNextToken();
    if (CurrentToken.state == State_Rbkt) {
        i--;
        return false;
    }
    GetNextToken();
    if (CurrentToken.state == State_Rbkt) {
        i -= 2;
        return true;
    } else {
        i -= 2;
        return false;
    }
}
