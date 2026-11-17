#include "push_swap.h"

t_option	*pre_option(t_option *o)
{
	o->simple = 0;
	o->medium = 0;
	o->complex = 0;
	o->adaptive = 0;
	o->bench = 0;
	return (o);
}

void	free_list(t_list *head)
{
	t_list	*next;

	while (head != NULL)
	{
		next = head->next;
		free(head);
		head = next;
	}
}

int	is_sorted(t_list *head)
{
	while (head != NULL && head->next != NULL)
	{
		if (head->arg > head->next->arg)
			return (0);
		head = head->next;
	}
	return (1);
}

void	run_algorithm(t_list **list, t_list **stack_b, t_option *op, t_stats *c)
{
	if ((*list)->next == NULL || is_sorted(*list))
		return ;
	if (op->simple == 1)
		ft_smeple(list, stack_b, c);
	else if (op->medium == 1)
		ft_medimum(list, stack_b, ft_lstsize(*list), c);
	else if (op->complex == 1)
		ft_complex(list, stack_b, c);
	else
		ft_adaptive(list, stack_b, c, op);
}

int	main_porpes(int argc, char *argv[], t_list *list, t_option *op)
{
	t_stats	c;
	t_list	*stack_b;

	init_stats(&c);
	stack_b = NULL;
	if (!cheek_is_arg_valid(argv, argc, pre_option(op), list))
	{
		free_list(list);
		write(2, "Error\n", 6);
		return (0);
	}
	c.disorder =  ft_disorder(list);
	run_algorithm(&list, &stack_b, op, &c);
	free_list(list);
	free_list(stack_b);
	if (op->bench == 1)
		ft_bench(&c, op);
	return (1);
}

int	main(int argc, char *argv[])
{
	t_option	op;
	t_list		*list;

	if (argc < 2)
		return (0);
	list = malloc(sizeof(t_list));
	if (list == NULL)
		return (1);
	list->next = NULL;
	if (!main_porpes(argc, argv, list, &op))
		return (1);
	return (0);
}