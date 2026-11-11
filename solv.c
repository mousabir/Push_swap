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

void	move_a(char *an, t_list **a, t_list **b, long n)
{
	if (ft_strcmp(an, "pb") == 0)
		push_node(a, b);
	else if (ft_strcmp(an, "ra") == 0)
		rotate_node(a);
	else if (ft_strcmp(an, "rra") == 0)
		reverse_node(a, n);
	else if (ft_strcmp(an, "pa") == 0)
		move_a("pb", b, a, n);
}

void	rotate_times(char *an, long times, t_list **a, long n)
{
	long	i;

	i = 0;
	while (i < times)
	{
		ft_printf("%s\n", an);
		move_a(an, a, NULL, n);
		i++;
	}
}

void	bring_to_top(long pos, long n, t_list **a, t_list **b)
{
	if (pos == 0)
	{
		ft_printf("pb\n");
		move_a("pb", a, b, n);
		return ;
	}
	else if (n - pos >= pos)
		rotate_times("ra", pos, a, n);
	else if (n - pos < pos)
		rotate_times("rra", n - pos, a, n);
	move_a("pb", a, b, n);
	ft_printf("pb\n");
}

void	ft_simeple(t_list *head, t_list *list_b)
{
	long	size;
	long	i;
	long	n;
	t_list	*hea;

	size = ft_lstsize(head);
	i = 0;
	n = size;
	while (head != NULL)
	{
		bring_to_top(smallest_position(head), n, &head, &list_b);
		n--;
	}
	while (i < size)
	{
		move_a("pa", &head, &list_b, size);
		ft_printf("pa\n");
		i++;
	}
	hea = head;
	while (hea != NULL)
	{
		ft_printf("%d\n", hea->arg);
		hea = hea->next;
	}
}