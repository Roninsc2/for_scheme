#include "vlist.h"
#include <iostream>


void VList::InsertAfter(VList* after)
{
    TPair* last = list.get();
    while (last->tail != nullptr) {
        if (last->tail->GetType() == PT_List) {
            last = ((TPairTypeList*)last->tail.get())->GetValue()->list.get();
        } else {
            //error
        }
    }
    last->tail.reset(new TPairTypeList(after));
}

void VList::InsertBefore(VList* before)
{
    TPair* last = before->list.get();
    while (last->tail != nullptr) {
        if (last->tail->GetType() == PT_List) {
            last = ((TPairTypeList*)last->tail.get())->GetValue()->list.get();
        } else {
            //error;
        }
    }
    last->tail.reset(new TPairTypeList(before));
}

void VList::ConvetToPair(TPairType* val) {
    TPair* last = list.get();
    while (last->tail != nullptr) {
        if (last->tail->GetType() == PT_List) {
            last = ((TPairTypeList*)last->tail.get())->GetValue()->list.get();
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
    while (last->tail != nullptr) {
        if (last->tail->GetType() == PT_List) {
            last = ((TPairTypeList*)last->tail.get())->GetValue()->list.get();
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
    while (last->tail != nullptr) {
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
        std::cout << ((TPairTypeInt*)expr)->GetValue() << " ";
        break;
    }
    case PT_Double: {
        std::cout << ((TPairTypeDouble*)expr)->GetValue() << " ";
        break;
    }
    case PT_Char: {
        std::cout << ((TPairTypeChar*)expr)->GetValue() << " ";
        break;
    }
    case PT_String: {
        std::cout << ((TPairTypeString*)expr)->GetValue() << " ";
        break;
    }
    case PT_Symbol: {
        std::cout << ((TPairTypeSymbol*)expr)->GetValue() << " ";
        break;
    }
    case PT_Bool: {
        std::cout << ((TPairTypeBool*)expr)->GetValue() << " ";
        break;
    }
    case PT_List: {
        ((TPairTypeList*)expr)->GetValue()->GetListData();
        break;
    }
    default:
        break;
    }
}


void VList::GetListData()
{
    TPair* last = list.get();
    if (last->head == nullptr && last->tail != nullptr) {
        GetPairData(last->tail.get());
        return;
    } else if (last->head == nullptr && last->tail == nullptr) {
       std::cout << "( )";
       return;
    }
    std::cout << "( ";
    while(last->tail != nullptr) {
        GetPairData(last->head.get());
        if (last->tail->GetType() == PT_List) {
            last = ((TPairTypeList*)last->tail.get())->GetValue()->list.get();
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




