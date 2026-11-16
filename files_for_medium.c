
#include "push_swap.h"

unsigned long find_postino(t_list *b, long index1)
{
    unsigned long pos;

    pos = 0;
    while (b != NULL)
    {
        if (b->index == index1)
            return (pos);
        pos++;
        b = b->next;
    }
    return (pos);
}
