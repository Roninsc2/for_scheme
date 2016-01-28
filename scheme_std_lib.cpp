#include <iostream>
#include "type.h"

TPairType* GetPairType(ExprType* expr) {
    switch (expr->Type) {
    case T_Bool: {
        return (new TPairTypeBool(dynamic_cast<BoolType*>(expr)->value));
    }
    case T_String : {
        return (new TPairTypeString(dynamic_cast<StringType*>(expr)->value));
    }
    case T_Symbol : {
        return (new TPairTypeSymbol(dynamic_cast<SymbolType*>(expr)->value));
    }
    case T_Char : {
        return (new TPairTypeChar(dynamic_cast<CharType*>(expr)->value));
    }
    case T_Int : {
        return (new TPairTypeInt(dynamic_cast<NumberIntType*>(expr)->value));
    }
    case T_Double : {
        return (new TPairTypeDouble(dynamic_cast<NumberDoubleType*>(expr)->value));
    }
    case T_List : {
        return (new TPairTypeList(dynamic_cast<ListType*>(expr)->value));
    }
    case T_Ident : {
        return GetPairType(dynamic_cast<IdentType*>(expr)->value.get());
    }
    default: {
        break;
        //error
    }
    }
}



ExprType* list(std::vector< std::shared_ptr<ExprType> >& exprs) {
    if (!exprs.size()) {
        return (new ListType(new VList(TPairTypePtr(nullptr))));
    } else {
        VList* list = new VList(TPairTypePtr(GetPairType(exprs.at(0).get())));
        for (size_t i = 1; i < exprs.size(); i++) {
            list->InsertAfter(std::shared_ptr<VList>(new VList(TPairTypePtr(GetPairType(exprs.at(i).get())))));
        }
        return (new ListType(list));
    }
}

ExprType* cons(std::vector<std::shared_ptr<ExprType>>& expr) {
    if (!expr.size()) {
        return (new ListType(new VList(TPairTypePtr(nullptr))));
    }
    VList* list = new VList(TPairTypePtr(GetPairType(expr.at(0).get())));
    list->ConvetToPair(GetPairType(expr.at(1).get()));
    return (new ListType(list));
}

ExprType* append(std::vector<std::shared_ptr<ExprType>>& expr) {
    if (!expr.size()) {
        return (new ListType(new VList(TPairTypePtr(nullptr))));
    }
    TPairType* list = GetPairType(expr.at(0).get());
    if (list->GetType() != PT_List) {
        //error
    }
    for (size_t i = 1; i < expr.size()-1; i++) {
        TPairType* addList = GetPairType(expr.at(i).get());
        if (addList->GetType() != PT_List) {
            //error
        } else {
            ((TPairTypeList*)list)->GetValue()->InsertAfter(((TPairTypeList*)addList)->GetValue());
        }
    }
    TPairType* lType = GetPairType(expr.at(expr.size()-1).get());
    if (lType->GetType() == PT_List && ((TPairTypeList*)lType)->GetValue()->isList()) {
        ((TPairTypeList*)list)->GetValue()->InsertAfter(((TPairTypeList*)lType)->GetValue());
    } else {
        ((TPairTypeList*)list)->GetValue()->ConvetToPair(lType);
    }
    return (new ListType(((TPairTypeList*)list)->GetValue().get()));
}



ExprType* plus(std::vector<std::shared_ptr<ExprType>>& expr) {
    double result = 0;
    if (!expr.size()) {
        //error
    } else {
        for (size_t i = 0; i < expr.size(); i++) {
            if (expr.at(i)->Type == T_Int) {
                result += dynamic_cast<NumberIntType*>(expr.at(i).get())->value;
            } else if (expr.at(i)->Type == T_Double) {
                result += dynamic_cast<NumberDoubleType*>(expr.at(i).get())->value;
            } else {
                //error
            }
        }
    }
    return (new NumberDoubleType(result));
}

ExprType* minus(std::vector<std::shared_ptr<ExprType>>& expr) {
    double result = 0;
    if (!expr.size()) {
        //error
    } else if (expr.size() > 1){
        if (expr.at(0)->Type == T_Int) {
            result = dynamic_cast<NumberIntType*>(expr.at(0).get())->value;
        } else if (expr.at(0)->Type == T_Double) {
            result = dynamic_cast<NumberDoubleType*>(expr.at(0).get())->value;
        } else {
            //error
        }
        for (size_t i = 1; i < expr.size(); i++) {
            if (expr.at(0)->Type == T_Int) {
                result -= dynamic_cast<NumberIntType*>(expr.at(i).get())->value;
            } else if (expr.at(0)->Type == T_Double) {
                result -= dynamic_cast<NumberDoubleType*>(expr.at(i).get())->value;
            } else {
                //error
            }
        }
    } else {
        if (expr.at(0)->Type == T_Int) {
            result -= dynamic_cast<NumberIntType*>(expr.at(0).get())->value;
        } else if (expr.at(0)->Type == T_Double) {
            result -= dynamic_cast<NumberDoubleType*>(expr.at(0).get())->value;
        } else {
            //error
        }
    }
    return (new NumberDoubleType(result));
}

ExprType* mult(std::vector<std::shared_ptr<ExprType>>& expr) {
    double result = 1;
    if (!expr.size()) {
        //error
    } else {
        for (size_t i = 0; i < expr.size(); i++) {
            if (expr.at(i)->Type == T_Int) {
                result *= dynamic_cast<NumberIntType*>(expr.at(i).get())->value;
            } else if (expr.at(i)->Type == T_Double) {
                result *= dynamic_cast<NumberDoubleType*>(expr.at(i).get())->value;
            } else {
                //error
            }
        }
    }
    return (new NumberDoubleType(result));
}

ExprType* defineFun(std::vector<std::shared_ptr<ExprType>>& expr) {
    if (expr.size() < 2) {
        //error
    } else if (expr.at(0)->Type == T_Ident && expr.size() == 2) {
        dynamic_cast<IdentType*>(expr.at(0).get())->value.reset(expr.at(1).get());
    }
}


ExprType* division(std::vector<std::shared_ptr<ExprType>>& expr) {
    double result = 1;
    if (!expr.size()) {
        //error
    } else if (expr.size() > 1){
        if (expr.at(0)->Type == T_Int) {
            result = dynamic_cast<NumberIntType*>(expr.at(0).get())->value;
        } else if (expr.at(0)->Type == T_Double) {
            result = dynamic_cast<NumberDoubleType*>(expr.at(0).get())->value;
        } else {
            //error
        }
        for (size_t i = 1; i < expr.size(); i++) {
            if (expr.at(i)->Type == T_Int) {
                result /= dynamic_cast<NumberIntType*>(expr.at(i).get())->value;
            } else if (expr.at(i)->Type == T_Double) {
                result /= dynamic_cast<NumberDoubleType*>(expr.at(i).get())->value;
            } else {
                //error
            }
        }
    } else {
        if (expr.at(0)->Type == T_Int) {
            result = 1.0 / dynamic_cast<NumberIntType*>(expr.at(0).get())->value;
        } else if (expr.at(0)->Type == T_Double) {
            result = 1.0 / dynamic_cast<NumberDoubleType*>(expr.at(0).get())->value;
        } else {
            //error
        }
    }
    return (new NumberDoubleType(result));
}

ExprType* equally(std::vector<std::shared_ptr<ExprType>>& expr) {
    if (expr.size() < 2) {
        //error
    } else {
        double result;
        if (expr.at(0)->Type == T_Int) {
            result = dynamic_cast<NumberIntType*>(expr.at(0).get())->value;
        } else if (expr.at(0)->Type == T_Double) {
            result = dynamic_cast<NumberDoubleType*>(expr.at(0).get())->value;
        } else {
            //error
        }
        for (size_t i = 1; i < expr.size(); i++) {
            if (expr.at(i)->Type == T_Int && dynamic_cast<NumberIntType*>(expr.at(i).get())->value != result) {
                return (new BoolType(false));
            } else if (expr.at(i)->Type == T_Double && dynamic_cast<NumberDoubleType*>(expr.at(i).get())->value != result) {
                return (new BoolType(false));
            } else {
                //error
            }
        }
        return (new BoolType(true));
    }

}




