#include "vlist.h"


void VList::InsertAfter(VList* after)
{
    TPair* last = list.get();
    if (last->head == nullptr) {
        last->head.reset(after->list->head.get());
        return;
    }
    while (last->tail != nullptr) {
        last = ((TPairTypeList*)last->tail.get())->GetValue()->list.get();
    }
    last->tail.reset(new TPairTypeList(after));
}

void VList::InsertBefore(VList* before)
{
    TPair* last = before->list.get();
    while (last->tail != nullptr) {
        last = ((TPairTypeList*)last->tail.get())->GetValue()->list.get();
    }
    last->tail.reset(new TPairTypeList(before));
}

void VList::ConvetToPair(TPairType* val) {
    isList = false;
    TPair* last = list.get();
    if (last->head == nullptr) {
        //error
    }
    while (last->tail != nullptr) {
        ((TPairTypeList*)last->tail.get())->GetValue()->isList = false;
        last = ((TPairTypeList*)last->tail.get())->GetValue()->list.get();
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
