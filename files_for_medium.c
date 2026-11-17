
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

float ft_disorder(t_list *a)
{
    long    mistakes;
    long pairs;
    t_list  *p;

    mistakes = 0;
    pairs = 0;
    while (a != NULL && a->next != NULL)
    {
        p = a->next;
        while (p != NULL)
        {
            if (a->arg > p->arg)
                 mistakes++;
            pairs++;
            p = p->next;
        }
        a = a->next;
    }
    if (pairs == 0)
        return (0);
    return ((float)mistakes / pairs);

}

void	ft_adaptive(t_list **a, t_list **b, t_stats *c, t_option *op)
{
    float disorder;

    disorder = ft_disorder(*a);
	if (disorder < 0.2f)
    {
		ft_smeple(a, b, c);
        op->simple = 1;
    }
	else if (disorder < 0.5f)
    {
		ft_medimum(a, b, ft_lstsize(*a), c);
        op->medium = 1;
    }
	else
    {
		ft_complex(a, b, c);
        op->complex = 1;
    }
}
