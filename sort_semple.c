/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sort_semple.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mousabir <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/22 17:24:45 by mousabir          #+#    #+#             */
/*   Updated: 2026/11/22 17:24:48 by mousabir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

t_bool chek_op(t_option *o)
{
	int fond;

	fond = 0;
	if (o->simple == 1)
		fond++;
	if (o->complex == 1)
		fond++;
	if (o->medium == 1)
		fond++;
	if (o->adaptive == 1)
		fond++;
	if (fond > 1)
		return (0);
	else
		return (1);
}

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

t_option	*pre_option(t_option *o)
{
	o->simple = 0;
	o->medium = 0;
	o->complex = 0;
	o->adaptive = 0;
	o->bench = 0;
	return (o);
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
