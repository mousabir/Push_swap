/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_args_utiles.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mousabir <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/22 17:22:48 by mousabir          #+#    #+#             */
/*   Updated: 2026/11/22 17:22:52 by mousabir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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

t_option	*pre_option(t_option *o)
{
	o->simple = 0;
	o->medium = 0;
	o->complex = 0;
	o->adaptive = 0;
	o->bench = 0;
	return (o);
}
