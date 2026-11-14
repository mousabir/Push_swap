#include "push_swap.h"

t_bool	chek_range(char *s)
{
	long	nb;
	int		sign;
	int		i;

	i = 0;
	sign = 1;
	nb = 0;
	if (s[i] == '-' || s[i] == '+')
	{
		if (s[i] == '-')
			sign = -1;
		i++;
	}
	while (s[i])
	{
		nb = nb * 10 + (s[i] - '0');
		if (nb * sign > INT_MAX || nb * sign < INT_MIN)
			return (0);
		i++;
	}
	return (1);
}

t_bool	has_dup(t_list *head)
{
	t_list	*cur;

	while (head != NULL)
	{
		cur = head->next;
		while (cur != NULL)
		{
			if (cur->arg == head->arg)
				return (1);
			cur = cur->next;
		}
		head = head->next;
	}
	return (0);
}
void creat_rank(t_list *head)
{
    int *array;
    t_list *i;
    int size;
    int j;

    size = ft_lstsize(head);
    if (size <= 0)
        return ;
    array = malloc(size * sizeof(int));
    if (array == NULL)
        return ;
    i = head;
    j = 0;
    while (i != NULL)
    {
        array[j] = i->arg;
        i = i->next;
        j++;
    }
    sort_array(array, size);
    asind_index(array, head, size);
    free(array);
}

static void scatter_chunks(t_list **head, t_list **stack_b, long size, t_stats *c)
{
    int (chunks_size), (current_min), (current_max), (pushed);

    chunks_size = ft_sqrt(size);
    current_min = 0;
    current_max = chunks_size;
    pushed = 0;
    while (*head != NULL)
    {
        if ((*head)->index >= current_min && (*head)->index < current_max)
        {
            bring_to_top(0, head, stack_b, c);
            pushed++;
            if (pushed >= chunks_size)
            {
                current_min = current_max;
                current_max += chunks_size;
                if (current_max > size)
                    current_max = size;
                pushed = 0;
            }
        }
        else
            rotate_times("ra", 1, head, c);
    }
}

void ft_medimum(t_list **head, t_list **stack_b, long size, t_stats *c)
{
    creat_rank(*head);
    scatter_chunks(head, stack_b, size, c);
    size--;
    while (size >= 0)
    {
        if ((*stack_b)->index == size)
        {
            ft_printf("pa\n");
            count_move(c, "pa");
            move_a("pa", head, stack_b, c);
            size--;
        }
        else
            rotate_times("rb", 1, stack_b, c);
    }
}