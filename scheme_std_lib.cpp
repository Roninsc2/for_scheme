#include <iostream>
#include "type.h"

TPairType* GetPairType(ExprType* expr) {
    switch (expr->Type) {
    case T_Bool: {
        return (new TPairTypeBool(((BoolType*)expr)->value));
    }
    case T_String : {
        return (new TPairTypeString(((StringType*)expr)->value));
    }
    case T_Symbol : {
        return (new TPairTypeSymbol(((SymbolType*)expr)->value));
    }
    case T_Char : {
        return (new TPairTypeChar(((CharType*)expr)->value));
    }
    case T_Int : {
        return (new TPairTypeInt(((NumberIntType*)expr)->value));
    }
    case T_Double : {
        return (new TPairTypeDouble(((NumberDoubleType*)expr)->value));
    }
    case T_List : {
        return (new TPairTypeList(((ListType*)expr)->value.get()));
    }
    case T_Ident : {
        return GetPairType(((IdentType*)expr)->value);
    }
    default: {
        break;
        //error
    }
    }
}



ExprType* list(std::vector< ExprType* > exprs) {
    if (!exprs.size()) {
        return (new ListType(new VList(TPairTypePtr(nullptr))));
    } else {
        VList* list = new VList(TPairTypePtr(GetPairType(exprs[0])));
        for (size_t i = 1; i < exprs.size(); i++) {
            list->InsertAfter(new VList(TPairTypePtr(GetPairType(exprs[i]))));
        }
        return (new ListType(list));
    }
}

ExprType* cons(std::vector <ExprType*> expr) {
    if (!expr.size()) {
        return (new ListType(new VList(TPairTypePtr(nullptr))));
    }
    VList* list = new VList(TPairTypePtr(GetPairType(expr[0])));
    list->ConvetToPair(GetPairType(expr[1]));
    return (new ListType(list));
}

ExprType* append(std::vector<ExprType*> expr) {
    if (!expr.size()) {
        return (new ListType(new VList(TPairTypePtr(nullptr))));
    }
    TPairType* list = GetPairType(expr[0]);
    if (list->GetType() != PT_List) {
        //error
    }
    for (size_t i = 1; i < expr.size()-1; i++) {
        TPairType* addList = GetPairType(expr[i]);
        if (addList->GetType() != PT_List) {
            //error
        } else {
            ((TPairTypeList*)list)->GetValue()->InsertAfter(((TPairTypeList*)addList)->GetValue());
        }
    }
    TPairType* lType = GetPairType(expr[expr.size()-1]);
    if (lType->GetType() == PT_List && ((TPairTypeList*)lType)->GetValue()->isList()) {
        ((TPairTypeList*)list)->GetValue()->InsertAfter(((TPairTypeList*)lType)->GetValue());
    } else {
        ((TPairTypeList*)list)->GetValue()->ConvetToPair(lType);
    }
    return (new ListType(((TPairTypeList*)list)->GetValue()));
}



ExprType* plus(std::vector<ExprType*> expr) {
    double result = 0;
    if (!expr.size()) {
        //error
    } else {
        for (size_t i = 0; i < expr.size(); i++) {
            if (expr[i]->Type == T_Int) {
                result += ((NumberIntType*)expr[i])->value;
            } else if (expr[i]->Type == T_Double) {
                result += ((NumberDoubleType*)expr[i])->value;
            } else {
                //error
            }
        }
    }
    return (new NumberDoubleType(result));
}

ExprType* minus(std::vector<ExprType*> expr) {
    double result = 0;
    if (!expr.size()) {
        //error
    } else if (expr.size() > 1){
        if (expr[0]->Type == T_Int) {
            result = ((NumberIntType*)expr[0])->value;
        } else if (expr[0]->Type == T_Double) {
            result = ((NumberDoubleType*)expr[0])->value;
        } else {
            //error
        }
        for (size_t i = 1; i < expr.size(); i++) {
            if (expr[i]->Type == T_Int) {
                result -= ((NumberIntType*)expr[i])->value;
            } else if (expr[i]->Type == T_Double) {
                result -= ((NumberDoubleType*)expr[i])->value;
            } else {
                //error
            }
        }
    } else {
        if (expr[0]->Type == T_Int) {
            result -= ((NumberIntType*)expr[0])->value;
        } else if (expr[0]->Type == T_Double) {
            result -= ((NumberDoubleType*)expr[0])->value;
        } else {
            //error
        }
    }
    return (new NumberDoubleType(result));
}

ExprType* mult(std::vector<ExprType*> expr) {
    double result = 1;
    if (!expr.size()) {
        //error
    } else {
        for (size_t i = 0; i < expr.size(); i++) {
            if (expr[i]->Type == T_Int) {
                result *= ((NumberIntType*)expr[i])->value;
            } else if (expr[i]->Type == T_Double) {
                result *= ((NumberDoubleType*)expr[i])->value;
            } else {
                //error
            }
        }
    }
    return (new NumberDoubleType(result));
}

ExprType* defineFun(std::vector<ExprType*> expr) {
    if (expr.size() < 2) {
        //error
    } else if (expr[0]->Type == T_Ident && expr.size() == 2) {
        ((IdentType*)expr[0])->value = expr[1];
    }
}


ExprType* division(std::vector<ExprType*> expr) {
    double result = 1;
    if (!expr.size()) {
        //error
    } else if (expr.size() > 1){
        if (expr[0]->Type == T_Int) {
            result = ((NumberIntType*)expr[0])->value;
        } else if (expr[0]->Type == T_Double) {
            result = ((NumberDoubleType*)expr[0])->value;
        } else {
            //error
        }
        for (size_t i = 1; i < expr.size(); i++) {
            if (expr[i]->Type == T_Int) {
                result /= ((NumberIntType*)expr[i])->value;
            } else if (expr[i]->Type == T_Double) {
                result /= ((NumberDoubleType*)expr[i])->value;
            } else {
                //error
            }
        }
    } else {
        if (expr[0]->Type == T_Int) {
            result = 1.0 / ((NumberIntType*)expr[0])->value;
        } else if (expr[0]->Type == T_Double) {
            result = 1.0 / ((NumberDoubleType*)expr[0])->value;
        } else {
            //error
        }
    }
    return (new NumberDoubleType(result));
}

ExprType* ifelse(std::vector<ExprType*> expr) {
    if (((BoolType*)expr[0])->value) {
        return expr[1];
    } else {
        return expr[2];
    }
}

ExprType* equally(std::vector<ExprType*> expr) {
    if (expr.size() < 2) {
        //error
    } else {
        double result;
        if (expr[0]->Type == T_Int) {
            result = ((NumberIntType*)expr[0])->value;
        } else if (expr[0]->Type == T_Double) {
            result = ((NumberDoubleType*)expr[0])->value;
        } else {
            //error
        }
        for (size_t i = 1; i < expr.size(); i++) {
            if (expr[i]->Type == T_Int && ((NumberIntType*)expr[i])->value != result) {
                return (new BoolType(false));
            } else if (expr[i]->Type == T_Double && ((NumberDoubleType*)expr[i])->value != result) {
                return (new BoolType(false));
            } else {
                //error
            }
        }
        return (new BoolType(true));
    }

}




