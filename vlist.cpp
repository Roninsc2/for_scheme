#include "vlist.h"
#include <iostream>


void VList::InsertAfter(std::shared_ptr<VList> after)
{
    TPair* last = list.get();
    while (last->tail) {
        if (last->tail->GetType() == PT_List) {
            last = dynamic_cast<TPairTypeList*>(last->tail.get())->GetValue()->list.get();
        } else {
            //error
        }
    }
    last->tail.reset(new TPairTypeList(after));
}

void VList::InsertBefore(std::shared_ptr<VList> before)
{
    TPair* last = before->list.get();
    while (last->tail) {
        if (last->tail->GetType() == PT_List) {
            last = dynamic_cast<TPairTypeList*>(last->tail.get())->GetValue()->list.get();
        } else {
            //error;
        }
    }
    last->tail.reset(new TPairTypeList(before));
}

void VList::ConvetToPair(TPairType* val) {
    TPair* last = list.get();
    while (last->tail) {
        if (last->tail->GetType() == PT_List) {
            last = dynamic_cast<TPairTypeList*>(last->tail.get())->GetValue()->list.get();
        } else {
            //error;
        }
    }
    last->tail.reset(val);
}

size_t VList::ListLength()
{
    TPair* last = list.get();
    size_t count = 1;
    while (last->tail) {
        if (last->tail->GetType() == PT_List) {
            last = dynamic_cast<TPairTypeList*>(last->tail.get())->GetValue()->list.get();
        } else {
            //error
        }
        count++;
    }
    return count;
}

bool VList::isList()
{
    TPair* last = list.get();
    while (last->tail) {
        if (last->tail->GetType() != PT_List) {
            return false;
        }
    }
    return true;
}

void GetPairData(TPairType* expr)
{
    switch(expr->Type) {
    case PT_Int: {
        std::cout << dynamic_cast<TPairTypeInt*>(expr)->GetValue() << " ";
        break;
    }
    case PT_Double: {
        std::cout << dynamic_cast<TPairTypeDouble*>(expr)->GetValue() << " ";
        break;
    }
    case PT_Char: {
        std::cout << dynamic_cast<TPairTypeChar*>(expr)->GetValue() << " ";
        break;
    }
    case PT_String: {
        std::cout << dynamic_cast<TPairTypeString*>(expr)->GetValue() << " ";
        break;
    }
    case PT_Symbol: {
        std::cout << dynamic_cast<TPairTypeSymbol*>(expr)->GetValue() << " ";
        break;
    }
    case PT_Bool: {
        std::cout << dynamic_cast<TPairTypeBool*>(expr)->GetValue() << " ";
        break;
    }
    case PT_List: {
        dynamic_cast<TPairTypeList*>(expr)->GetValue()->GetListData();
        break;
    }
    default:
        break;
    }
}


void VList::GetListData()
{
    TPair* last = list.get();
    if (last->head == nullptr && last->tail) {
        GetPairData(last->tail.get());
        return;
    } else if (last->head == nullptr && last->tail == nullptr) {
       std::cout << "( )";
       return;
    }
    std::cout << "( ";
    while(last->tail) {
        GetPairData(last->head.get());
        if (last->tail->GetType() == PT_List) {
            last = dynamic_cast<TPairTypeList*>(last->tail.get())->GetValue()->list.get();
        } else {
            std::cout << ". ";
            GetPairData(last->tail.get());
            std::cout << ") ";
            return;
        }
    }
    GetPairData(last->head.get());
    std::cout << ") ";
}




