#include "vlist.h"


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
    if (last->head == nullptr) {
        //error
    }
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
        last = ((TPairTypeList*)last->tail.get())->GetValue()->list.get();
        count++;
    }
    return count;
}
