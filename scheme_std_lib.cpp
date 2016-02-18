#include <iostream>
#include "type.h"

TPairType* GetPairType(ExprType* expr) {
    switch (expr->Type) {
    case T_Bool: {
        return (new TPairTypeBool(dynamic_cast<BoolType*>(expr)->GetValue()));
    }
    case T_String : {
        return (new TPairTypeString(dynamic_cast<StringType*>(expr)->GetValue()));
    }
    case T_Symbol : {
        return (new TPairTypeSymbol(dynamic_cast<SymbolType*>(expr)->GetValue()));
    }
    case T_Char : {
        return (new TPairTypeChar(dynamic_cast<CharType*>(expr)->GetValue()));
    }
    case T_Int : {
        return (new TPairTypeInt(dynamic_cast<NumberIntType*>(expr)->GetValue()));
    }
    case T_Double : {
        return (new TPairTypeDouble(dynamic_cast<NumberDoubleType*>(expr)->GetValue()));
    }
    case T_List : {
        return (new TPairTypeList(dynamic_cast<ListType*>(expr)->GetValue()));
    }
    case T_Ident : {
        return GetPairType(dynamic_cast<IdentType*>(expr)->GetValue().get());
    }
    default: {
        break;
        //error
    }
    }
}



ExprType* GetList(std::vector< std::shared_ptr<ExprType> >& exprs) {
    if (!exprs.size()) {
        return (new ListType(new VList(TPairTypePtr(nullptr))));
    } else {
        VList* list = new VList(TPairTypePtr(GetPairType(exprs.at(exprs.size()-1).get())));
        for (int i = exprs.size()-2; i >= 0; i--) {
            list->InsertAfter(std::shared_ptr<VList>(new VList(TPairTypePtr(GetPairType(exprs.at(i).get())))));
        }
        return (new ListType(list));
    }
}

ExprType* cons(std::vector<std::shared_ptr<ExprType>>& expr) {
    if (!expr.size()) {
        return (new ListType(new VList(TPairTypePtr(nullptr))));
    }
    VList* list = new VList(TPairTypePtr(GetPairType(expr.at(1).get())));
    list->ConvetToPair(GetPairType(expr.at(0).get()));
    return (new ListType(list));
}

ExprType* append(std::vector<std::shared_ptr<ExprType>>& expr) {
    if (!expr.size()) {
        return (new ListType(new VList(TPairTypePtr(nullptr))));
    }
    std::shared_ptr<TPairType> list(GetPairType(expr.at(expr.size()-1).get()));
    if (list->GetType() != PT_List) {
        //error
    }
    for (size_t i = expr.size()-2; i >= 1; i--) {
        std::shared_ptr<TPairType> addList(GetPairType(expr.at(i).get()));
        if (addList->GetType() != PT_List) {
            //error
        } else {
            ((TPairTypeList*)list.get())->GetValue()->InsertAfter(((TPairTypeList*)addList.get())->GetValue());
        }
    }
    std::shared_ptr<TPairType> lType(GetPairType(expr.at(0).get()));
    if (lType->GetType() == PT_List && ((TPairTypeList*)lType.get())->GetValue()->isList()) {
        ((TPairTypeList*)list.get())->GetValue()->InsertAfter(((TPairTypeList*)lType.get())->GetValue());
    } else {
        ((TPairTypeList*)list.get())->GetValue()->ConvetToPair(lType.get());
    }
    return (new ListType(((TPairTypeList*)list.get())->GetValue()));
}



ExprType* plus(std::vector<std::shared_ptr<ExprType>>& expr) {
    double result = 0;
    if (!expr.size()) {
        //error
    } else {
        for (int i = expr.size()-1; i >= 0; i--) {
            if (expr.at(i)->Type == T_Int) {
                result += dynamic_cast<NumberIntType*>(expr.at(i).get())->GetValue();
            } else if (expr.at(i)->Type == T_Double) {
                result += dynamic_cast<NumberDoubleType*>(expr.at(i).get())->GetValue();
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
        if (expr.at(expr.size()-1)->Type == T_Int) {
            result = dynamic_cast<NumberIntType*>(expr.at(expr.size()-1).get())->GetValue();
        } else if (expr.at(expr.size()-1)->Type == T_Double) {
            result = dynamic_cast<NumberDoubleType*>(expr.at(expr.size()-1).get())->GetValue();
        } else {
            //error
        }
        for (int i = expr.size()-2; i >= 0; i--) {
            if (expr.at(i)->Type == T_Int) {
                result -= dynamic_cast<NumberIntType*>(expr.at(i).get())->GetValue();
            } else if (expr.at(i)->Type == T_Double) {
                result -= dynamic_cast<NumberDoubleType*>(expr.at(i).get())->GetValue();
            } else {
                //error
            }
        }
    } else {
        if (expr.at(0)->Type == T_Int) {
            result -= dynamic_cast<NumberIntType*>(expr.at(0).get())->GetValue();
        } else if (expr.at(0)->Type == T_Double) {
            result -= dynamic_cast<NumberDoubleType*>(expr.at(0).get())->GetValue();
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
        for (int i = expr.size()-1; i >= 0; i--) {
            if (expr.at(i)->Type == T_Int) {
                result *= dynamic_cast<NumberIntType*>(expr.at(i).get())->GetValue();
            } else if (expr.at(i)->Type == T_Double) {
                result *= dynamic_cast<NumberDoubleType*>(expr.at(i).get())->GetValue();
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
    } else if (expr.at(1)->Type == T_Ident && expr.size() == 2) {
        dynamic_cast<IdentType*>(expr.at(1).get())->GetValue() = expr.at(0);
    }
}


ExprType* division(std::vector<std::shared_ptr<ExprType>>& expr) {
    double result = 1;
    if (!expr.size()) {
        //error
    } else if (expr.size() > 1){
        if (expr.at(expr.size()-1)->Type == T_Int) {
            result = dynamic_cast<NumberIntType*>(expr.at(expr.size()-1).get())->GetValue();
        } else if (expr.at(expr.size()-1)->Type == T_Double) {
            result = dynamic_cast<NumberDoubleType*>(expr.at(expr.size()-1).get())->GetValue();
        } else {
            //error
        }
        for (int i = expr.size()-2; i >= 0; i--) {
            if (expr.at(i)->Type == T_Int) {
                result /= dynamic_cast<NumberIntType*>(expr.at(i).get())->GetValue();
            } else if (expr.at(i)->Type == T_Double) {
                result /= dynamic_cast<NumberDoubleType*>(expr.at(i).get())->GetValue();
            } else {
                //error
            }
        }
    } else {
        if (expr.at(0)->Type == T_Int) {
            result = 1.0 / dynamic_cast<NumberIntType*>(expr.at(0).get())->GetValue();
        } else if (expr.at(0)->Type == T_Double) {
            result = 1.0 / dynamic_cast<NumberDoubleType*>(expr.at(0).get())->GetValue();
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
        if (expr.at(expr.size()-1)->Type == T_Int) {
            result = dynamic_cast<NumberIntType*>(expr.at(expr.size()-1).get())->GetValue();
        } else if (expr.at(expr.size()-1)->Type == T_Double) {
            result = dynamic_cast<NumberDoubleType*>(expr.at(expr.size()-1).get())->GetValue();
        } else {
            //error
        }
        for (int i = expr.size()-2; i >= 0; i--) {
            if (expr.at(i)->Type == T_Int && dynamic_cast<NumberIntType*>(expr.at(i).get())->GetValue() != result) {
                return (new BoolType(false));
            } else if (expr.at(i)->Type == T_Double && dynamic_cast<NumberDoubleType*>(expr.at(i).get())->GetValue() != result) {
                return (new BoolType(false));
            } else {
                //error
            }
        }
        return (new BoolType(true));
    }

}




