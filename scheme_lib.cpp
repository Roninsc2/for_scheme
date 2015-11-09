#include "scheme_lib.h"
#include <iostream>

TPairType* GetPairType(ExprAST* expr) {
    switch (expr->Type) {
    case AT_Bool: {
        return (new TPairTypeBool(((BoolAST*)expr)->value));
    }
    case AT_String : {
        return (new TPairTypeString(((StringAST*)expr)->value));
    }
    case AT_Symbol : {
        return (new TPairTypeSymbol(((SymbolAST*)expr)->value));
    }
    case AT_Char : {
        return (new TPairTypeChar(((CharAST*)expr)->value));
    }
    case AT_Int : {
        return (new TPairTypeInt(((NumberIntAST*)expr)->value));
    }
    case AT_Double : {
        return (new TPairTypeDouble(((NumberDoubleAST*)expr)->value));
    }
    case AT_Func : {
        if (((CallExprAST*)expr)->Callee == "list") {
            return (new TPairTypeList(list(((CallExprAST*)expr)->Args)));
        } else {
            //error
        }
    }
    default: {
        break;
        //error
    }
    }
}



VList* list(std::vector< ExprAST* > exprs) {
    if (!exprs.size()) {
        return (new VList(TPairTypePtr(nullptr)));
    } else {
        VList* list = new VList(TPairTypePtr(GetPairType(exprs[0])));
        for (size_t i = 1; i < exprs.size(); i++) {
            if (exprs[i]->Type == AT_Symbol && ((SymbolAST*)exprs[i])->value == ".") {
               list->ConvetToPair(GetPairType(exprs[i+1]));
               return list;
            } else {
                list->InsertAfter(new VList(TPairTypePtr(GetPairType(exprs[i]))));
            }
        }
        return list;
    }
}

void plus(std::vector<ExprAST*> expr) {
    double result = 0;
    if (!expr.size()) {
        //error
    } else {
        for (size_t i = 0; i < expr.size(); i++) {
            if (expr[i]->Type == AT_Int) {
                result += ((NumberIntAST*)expr[i])->value;
            } else if (expr[i]->Type == AT_Double) {
                result += ((NumberDoubleAST*)expr[i])->value;
            } else {
                //error
            }
        }
    }
    std::cout << result << "\n";
}



