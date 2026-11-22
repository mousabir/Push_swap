/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sort_medium.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mousabir <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/22 17:24:14 by mousabir          #+#    #+#             */
/*   Updated: 2026/11/22 17:24:16 by mousabir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

void	sort_array(int *array, int size)
{
	int	i;
	int	j;
	int	box;

	i = 0;
	while (i < size)
	{
		j = 0;
		while (j < size)
		{
			if (array[i] < array[j])
			{
				box = array[i];
				array[i] = array[j];
				array[j] = box;
			}
			j++;
		}
		i++;
	}
}

void	asind_index(int *array, t_list *head, int size)
{
	int	i;

	while (head != NULL)
	{
		i = 0;
		while (i < size)
		{
			if (head->arg == array[i])
				head->index = i;
			i++;
		}
		head = head->next;
	}
}

void	creat_rank(t_list *head)
{
	int		*array;
	t_list	*i;
	int		size;
	int		j;

	size = ft_lstsize(head);
	if (size <= 0)
		return ;
	array = malloc(size * sizeof(int));
	if (array == NULL)
		return ;
	i = head;
	j = 0;
	while (i != NULL)
	{
		array[j] = i->arg;
		i = i->next;
		j++;
	}
	sort_array(array, size);
	asind_index(array, head, size);
	free(array);
}

static void	scatter_chunks(t_list **a, t_list **b, long size, t_stats *c)
{
	int	chunk;
	int	min;
	int	max;

	chunk = ft_sqrt(size);
	min = 0;
	max = chunk;
	while (*a != NULL)
	{
		if ((*a)->index >= min && (*a)->index < max)
		{
			bring_to_top(0, a, b, c);
			if (ft_lstsize(*b) >= max)
			{
				min = max;
				max += chunk;
			}
		}
		else
			rotate_times("ra", 1, a, c);
	}
}

void	ft_medimum(t_list **head, t_list **stack_b, long size, t_stats *c)
{
	long	pos;
	long	len;

	creat_rank(*head);
	scatter_chunks(head, stack_b, size, c);
	size--;
	while (size >= 0)
	{
		len = ft_lstsize(*stack_b);
		pos = find_postino(*stack_b, size);
		if (pos == 0)
		{
			ft_printf("pa\n");
			count_move(c, "pa");
			move_a("pa", head, stack_b, c);
			size--;
		}
		else if (len - pos <= pos)
		{
			c->size = len;
			rotate_times("rrb", len - pos, stack_b, c);
		}
		else
			rotate_times("rb", pos, stack_b, c);
	}
}
