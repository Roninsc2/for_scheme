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
            root.push_back(ParseCallExprAST());
            break;
        }
        case State_Lbkt: {
            continue;
            break;
        }
        case State_Rbkt: {
            continue; //error
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
    case State_Ident:
        return (new IdentAST(CurrentToken.value));
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
                return (new ListAST(ParseList()));
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
        break;//error
    }
}

VList* TParser::ParseList() {
    GetNextToken();
    VList* list = GetPairType();
    while (1) {
        GetNextToken();
        if (CurrentToken.state == State_Rbkt) {
            break;
        }
        list->InsertAfter(GetPairType());
    }
    return list;
}

VList* TParser::GetPairType() {
    switch (CurrentToken.state) {
    case State_Lbkt : {
        return (ParseList());
    }
    case State_Ident:
        return (new VList(TPairTypePtr(new TPairTypeSymbol(CurrentToken.value))));
    case State_Bool: {
        if (CurrentToken.value == "#t")  {
            return (new VList(TPairTypePtr(new TPairTypeBool(true))));
        } else {
            return (new VList(TPairTypePtr(new TPairTypeBool(false))));
        }
    }
    case State_String : {
        std::string val = CurrentToken.value;
        val.erase(0,1);
        val.erase(val.size()-1, val.size());
        return (new VList(TPairTypePtr(new TPairTypeString(val))));
    }
    case State_Symbol : {

        std::string val = CurrentToken.value;
        if (val.length() != 1) {
            val.erase(0,1);
            return (new VList(TPairTypePtr(new TPairTypeSymbol(val))));
        } else {
            GetNextToken();
            if (CurrentToken.state == State_Lbkt) {
                return (ParseList());
            }
        }
    }
    case State_Char : {
        char val  = CurrentToken.value[2];
        return (new VList(TPairTypePtr(new TPairTypeChar(val))));
    }
    case State_Int : {
        int val  = std::stoi(CurrentToken.value);
        return (new VList(TPairTypePtr(new TPairTypeInt(val))));
    }
    case State_Double : {
        double val  =  std::stod(CurrentToken.value);
        return (new VList(TPairTypePtr(new TPairTypeDouble(val))));
    }
    default: {
        //error
    }
    }
}

