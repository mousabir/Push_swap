/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   checker_helper.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ilazerou <ilazerou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/20 16:12:09 by ilazerou          #+#    #+#             */
/*   Updated: 2026/11/20 16:50:03 by ilazerou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "checker.h"

static void	swap_stack(t_list *stack)
{
	int	tmp;

	if (!stack || !stack->next)
		return ;
	tmp = stack->arg;
	stack->arg = stack->next->arg;
	stack->next->arg = tmp;
}

static void	push_stack(t_list **from, t_list **to)
{
	t_list	*node;

	if (!*from)
		return ;
	node = *from;
	*from = node->next;
	node->next = *to;
	*to = node;
}

static void	rotate_stack(t_list **stack)
{
	t_list	*first;
	t_list	*last;

	if (!*stack || !(*stack)->next)
		return ;
	first = *stack;
	*stack = first->next;
	first->next = NULL;
	last = *stack;
	while (last->next)
		last = last->next;
	last->next = first;
}

static void	reverse_stack(t_list **stack)
{
	t_list	*prev;
	t_list	*last;

	if (!*stack || !(*stack)->next)
		return ;
	prev = NULL;
	last = *stack;
	while (last->next)
	{
		prev = last;
		last = last->next;
	}
	prev->next = NULL;
	last->next = *stack;
	*stack = last;
}

static int	execute_push_swap(char *line, t_list **a, t_list **b)
{
	if (!ft_strcmp(line, "sa"))
		swap_stack(*a);
	else if (!ft_strcmp(line, "sb"))
		swap_stack(*b);
	else if (!ft_strcmp(line, "ss"))
	{
		swap_stack(*a);
		swap_stack(*b);
	}
	else if (!ft_strcmp(line, "pa"))
		push_stack(b, a);
	else if (!ft_strcmp(line, "pb"))
		push_stack(a, b);
	else
		return (0);
	return (1);
}

static int	execute_rotate(char *line, t_list **a, t_list **b)
{
	if (!ft_strcmp(line, "ra"))
		rotate_stack(a);
	else if (!ft_strcmp(line, "rb"))
		rotate_stack(b);
	else if (!ft_strcmp(line, "rr"))
	{
		rotate_stack(a);
		rotate_stack(b);
	}
	else if (!ft_strcmp(line, "rra"))
		reverse_stack(a);
	else if (!ft_strcmp(line, "rrb"))
		reverse_stack(b);
	else if (!ft_strcmp(line, "rrr"))
	{
		reverse_stack(a);
		reverse_stack(b);
	}
	else
		return (0);
	return (1);
}

int	checker_execute(char *line, t_list **a, t_list **b)
{
	if (execute_push_swap(line, a, b))
		return (1);
	return (execute_rotate(line, a, b));
}