#include "vlist.h"


void VList::InsertAfter(VList* after)
{
    VList* last = this;
    while (last->next != nullptr) {
        last = last->next.get();
    }
    last->next.reset(after);
}

void VList::InsertBefore(VList* before)
{
    VList* last = before;
    while (last->next != nullptr) {
        last = last->next.get();
    }
    last->next.reset(this);;
}

size_t VList::ListLength()
{
    VList* last =  this;
    size_t count = 0;
    while (last != nullptr) {
        last = last->next.get();
        count++;
    }
    return count;
}
