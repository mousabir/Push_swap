/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cheker_bonus_utiles.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mousabir <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/22 17:20:51 by mousabir          #+#    #+#             */
/*   Updated: 2026/11/22 17:21:00 by mousabir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

int	execute_push_swap(char *line, t_list **a, t_list **b)
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

void	swap_stack(t_list *stack)
{
	int	tmp;

	if (!stack || !stack->next)
		return ;
	tmp = stack->arg;
	stack->arg = stack->next->arg;
	stack->next->arg = tmp;
}
