#include "push_swap.h"

long	smallest_position(t_list *head)
{
	int		min;
	long	pos;
	long	i;

	if (head == NULL)
		return (-1);
	i = 0;
	pos = 0;
	min = head->arg;
	while (head != NULL)
	{
		if (min > head->arg)
		{
			min = head->arg;
			pos = i;
		}
		head = head->next;
		i++;
	}
	return (pos);
}

void	move_a(char *an, t_list **a, t_list **b, t_stats *c)
{
	if (ft_strcmp(an, "pb") == 0)
		push_node(a, b);
	else if (ft_strcmp(an, "ra") == 0)
		rotate_node(a);
	else if (ft_strcmp(an, "rra") == 0)
		reverse_node(a, c->size);
	else if (ft_strcmp(an, "pa") == 0)
		move_a("pb", b, a, c);
	else if (ft_strcmp(an, "rb") == 0)
		move_a("ra", a, b, c);
}

void	rotate_times(char *an, long times, t_list **a, t_stats *c)
{
	long	i;

	i = 0;
	while (i < times)
	{
		ft_printf("%s\n", an);
		count_move(c, an);
		move_a(an, a, NULL, c);
		i++;
	}
}

void	bring_to_top(long pos, t_list **a, t_list **b, t_stats *c)
{
	if (pos == 0)
	{
		ft_printf("pb\n");
		count_move(c, "pb");
		move_a("pb", a, b, c);
		return ;
	}
	else if (c->size - pos >= pos)
		rotate_times("ra", pos, a, c);
	else if (c->size - pos < pos)
		rotate_times("rra", c->size - pos, a, c);
	move_a("pb", a, b, c);
	ft_printf("pb\n");
	count_move(c, "pb");
}

void	ft_smeple(t_list **head, t_list **list_b, t_stats *c)
{
	long	size;
	long	i;

	size = ft_lstsize(*head);
	c->size = size;
	i = 0;
	while (*head != NULL)
	{
		bring_to_top(smallest_position(*head), head, list_b, c);
		c->size--;
	}
	while (i < size)
	{
		move_a("pa", head, list_b, c);
		ft_printf("pa\n");
		count_move(c, "pa");
		i++;
	}
}