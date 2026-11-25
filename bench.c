/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bench.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mousabir <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/22 17:17:53 by mousabir          #+#    #+#             */
/*   Updated: 2026/11/22 17:18:04 by mousabir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

static void	ft_print_adaptive(t_option *op)
{
	if (op->simple == 1)
		ft_dprintf("Strategy : Adaptive O(n^2)\n");
	else if (op->medium == 2)
		ft_dprintf("Strategy : Adaptive O(n*sqrt(n))\n");
	else
		ft_dprintf("Strategy : Adaptive O(n log n)\n");
}

static void	ft_print_disorder(t_stats *c)
{
	int	val;

	val = (int)(c->disorder * 10000 + 0.5f);
	ft_dprintf("Disorder : ");
	ft_dprintf("%d.", val / 100);
	if (val % 100 < 10)
		ft_dprintf("0");
	ft_dprintf("%d%%\n", val % 100);
}

static void	ft_print_strategy(t_option *op)
{
	if (op->adaptive == 1)
		ft_print_adaptive(op);
	else if (op->simple == 1)
		ft_dprintf("Strategy : Simple O(n^2)\n");
	else if (op->medium == 1)
		ft_dprintf("Strategy : Medium O(n*sqrt(n))\n");
	else if (op->complex == 1)
		ft_dprintf("Strategy : Complex O(n log n)\n");
}

static void	ft_print_counts(t_stats *c)
{
	ft_dprintf("sa : %d\nsb : %d\nss : %d\n", c->sa, c->sb, c->ss);
	ft_dprintf("pa : %d\npb : %d\n", c->pa, c->pb);
	ft_dprintf("ra : %d\nrb : %d\nrr : %d\n", c->ra, c->rb, c->rr);
	ft_dprintf("rra : %d\nrrb : %d\nrrr : %d\n", c->rra, c->rrb, c->rrr);
}

void	ft_bench(t_stats *c, t_option *op)
{
	ft_print_disorder(c);
	ft_print_strategy(op);
	ft_dprintf("total number of operations : %d\n", c->total);
	ft_print_counts(c);
}
