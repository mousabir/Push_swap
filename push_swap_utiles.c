#include "push_swap.h"

t_list	*find_node(t_list *stack, long pos)
{
	int	i;

	i = 0;
	while (stack != NULL)
	{
		if (i == pos)
			return (stack);
		stack = stack->next;
		i++;
	}
	return (NULL);
}

long	ft_lstsize(t_list *lst)
{
	long	size;

	size = 0;
	while (lst != NULL)
	{
		size++;
		lst = lst->next;
	}
	return (size);
}

void	push_node(t_list **a, t_list **b)
{
	t_list	*head;

	head = (*a)->next;
	ft_lstadd_front(b, *a);
	*a = head;
}

void	rotate_node(t_list **a)
{
	t_list	*head;

	head = find_node(*a, 0);
	*a = (*a)->next;
	head->next = NULL;
	ft_lstadd_back(a, head);
}

void	reverse_node(t_list **a, long n)
{
	t_list	*box;
	t_list	*head;

	box = find_node(*a, n - 2);
	head = box->next;
	box->next = NULL;
	ft_lstadd_front(a, head);
}