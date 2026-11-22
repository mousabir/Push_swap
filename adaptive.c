/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   adaptive.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mousabir <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/22 17:18:18 by mousabir          #+#    #+#             */
/*   Updated: 2026/11/22 17:18:28 by mousabir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

float	ft_disorder(t_list *a)
{
	long	mistakes;
	long	pairs;
	t_list	*p;

	mistakes = 0;
	pairs = 0;
	while (a != NULL && a->next != NULL)
	{
		p = a->next;
		while (p != NULL)
		{
			if (a->arg > p->arg)
				mistakes++;
			pairs++;
			p = p->next;
		}
		a = a->next;
	}
	if (pairs == 0)
		return (0);
	return ((float)mistakes / pairs);
}

void	ft_adaptive(t_list **a, t_list **b, t_stats *c, t_option *op)
{
	float	disorder;

	disorder = ft_disorder(*a);
	if (disorder < 0.2f)
	{
		ft_smeple(a, b, c);
		op->simple = 1;
	}
	else if (disorder < 0.5f)
	{
		ft_medimum(a, b, ft_lstsize(*a), c);
		op->medium = 1;
	}
	else
	{
		ft_complex(a, b, c);
		op->complex = 1;
	}
}
