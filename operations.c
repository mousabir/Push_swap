/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   operations.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mousabir <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/22 17:22:13 by mousabir          #+#    #+#             */
/*   Updated: 2026/11/22 17:22:20 by mousabir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

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
	else if (ft_strcmp(an, "rrb") == 0)
		move_a("rra", a, b, c);
	else if (ft_strcmp(an, "rr") == 0)
	{
		move_a("ra", a, b, c);
		move_a("rb", a, b, c);
	}
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
// chek push_utiles.c !! 
