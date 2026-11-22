/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sort_complex.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mousabir <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/22 17:23:53 by mousabir          #+#    #+#             */
/*   Updated: 2026/11/22 17:23:55 by mousabir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

int	bit_count(long max_rank)
{
	int	count;

	count = 0;
	while (max_rank > 0)
	{
		max_rank = max_rank / 2;
		count++;
	}
	return (count);
}

void	push_back_all(t_list **a, t_list **b, t_stats *c)
{
	while (*b != NULL)
	{
		ft_printf("pa\n");
		count_move(c, "pa");
		move_a("pa", a, b, c);
	}
}

void	radix_pass(t_list **a, t_list **b, t_stats *c, int bit)
{
	int	size;

	size = ft_lstsize(*a);
	while (size > 0)
	{
		if ((((*a)->index >> bit) & 1) == 0)
		{
			ft_printf("pb\n");
			count_move(c, "pb");
			move_a("pb", a, b, c);
		}
		else
			rotate_times("ra", 1, a, c);
		size--;
	}
	push_back_all(a, b, c);
}

void	ft_complex(t_list **a, t_list **b, t_stats *c)
{
	int	bit;
	int	max_bit;

	creat_rank(*a);
	max_bit = bit_count(ft_lstsize(*a) - 1);
	bit = 0;
	while (bit < max_bit)
	{
		radix_pass(a, b, c, bit);
		bit++;
	}
}
