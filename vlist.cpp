#include "vlist.h"


void VList::InsertAfter(VList* after)
{
    TPair* last = list.get();
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
