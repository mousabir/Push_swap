/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sort_medium_utils.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mousabir <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/22 17:24:27 by mousabir          #+#    #+#             */
/*   Updated: 2026/11/22 17:24:29 by mousabir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

size_t	find_postino(t_list *b, long index1)
{
	size_t	pos;

	pos = 0;
	while (b != NULL)
	{
		if (b->index == index1)
			return (pos);
		pos++;
		b = b->next;
	}
	return (pos);
}

int	ft_sqrt(int nb)
{
	long long	i;

	if (nb <= 0)
		return (0);
	i = 0;
	while (i * i <= nb)
		i++;
	return (i - 1);
}
