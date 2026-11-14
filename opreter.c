#include "push_swap.h"

void	init_stats(t_stats *c)
{
	c->size = 0;
	c->total = 0;
	c->sa = 0;
	c->sb = 0;
	c->ss = 0;
	c->pa = 0;
	c->pb = 0;
	c->ra = 0;
	c->rb = 0;
	c->rr = 0;
	c->rra = 0;
	c->rrb = 0;
	c->rrr = 0;
}

void	count_move(t_stats *c, char *an)
{
	if (ft_strcmp(an, "sa") == 0)
		c->sa++;
	else if (ft_strcmp(an, "sb") == 0)
		c->sb++;
	else if (ft_strcmp(an, "ss") == 0)
		c->ss++;
	else if (ft_strcmp(an, "pa") == 0)
		c->pa++;
	else if (ft_strcmp(an, "pb") == 0)
		c->pb++;
	else if (ft_strcmp(an, "ra") == 0)
		c->ra++;
	else if (ft_strcmp(an, "rb") == 0)
		c->rb++;
	else if (ft_strcmp(an, "rr") == 0)
		c->rr++;
	else if (ft_strcmp(an, "rra") == 0)
		c->rra++;
	else if (ft_strcmp(an, "rrb") == 0)
		c->rrb++;
	else if (ft_strcmp(an, "rrr") == 0)
		c->rrr++;
	c->total++;
}

int ft_sqrt(int nb)
{
    long long i;

    if (nb <= 0)
        return (0);
    i = 0;
    while (i * i <= nb)
        i++;
    return (i - 1);
}

void sort_array(int *array, int size)
{
	int (i), (j), (box);
	i = 0;
	while (i < size)
	{
		j = 0;
		while (j < size)
		{
			if (array[i] < array[j])
			{
				box = array[i];
				array[i] = array[j];
				array[j] = box;
			}
			j++;
		}
		i++;
	}
}

void asind_index(int *array, t_list *head, int size)
{
	int i ;
	while (head != NULL)
	{
		i = 0;
		while (i < size)
		{
			if (head->arg == array[i])
				head->index = i;
			i++;
		}
		head = head->next;
	}
}

/*#include <stdio.h>

int main(int argc, char *argv[])
{
	int array[10];
	t_option op;
	t_list *head;

	head = malloc(sizeof(t_list));
	head->next = NULL;
	int i;
	i = 0;
	cheek_is_arg_valid(argv, argc, pre_option(&op), head);
	creat_rank(head, 5);
	while (head != NULL)
	{
		printf("%d = %d\n", head->arg, head->index);
		head = head->next;
	}
	return (0);
}*/