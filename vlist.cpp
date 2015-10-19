#include "vlist.h"


void VList::InsertAfter(VList& after)
{
    VList* last = this;
    while (last->next != nullptr) {
        last = last->next;
    }
    last->next = &after;
}

void VList::InsertBefore(VList &before)
{
    VList* last = &before;
    while (last->next != nullptr) {
        last = last->next;
    }
    last->next = this;
}

size_t VList::ListLength()
{
    VList* last =  this;
    size_t count = 0;
    while (last != nullptr) {
        last = last->next;
        count++;
    }
    return count;
}
