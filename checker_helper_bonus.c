/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   checker_heper_bonus.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mousabir <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/22 17:19:49 by mousabir          #+#    #+#             */
/*   Updated: 2026/11/22 17:20:23 by mousabir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "checker.h"

static int	get_sign(char *s, int *i)//
{
	int	sign;

	*i = 0;
	sign = 1;
	if (s[*i] == '-' || s[*i] == '+')
	{
		if (s[*i] == '-')
			sign = -1;
		(*i)++;
	}
	return (sign);
}

static int	to_int(char *s, int *value)
{
	long long	n;
	long long	limit;
	int			sign;
	int			i;

	sign = get_sign(s, &i);
	n = 0;
	if (s[i] == '\0')
		return (0);
	while (s[i])
	{
		if (s[i] < '0' || s[i] > '9')
			return (0);
		if (n * sign > INT_MAX || n * sign < INT_MIN)
			return (0);
		n = n * 10 + s[i++] - '0';
	}
	*value = (n * sign);
	return (1);
}

static int	duplicate(t_list *a)
{
	t_list	*current;
	t_list	*check;

	current = a;
	while (current)
	{
		check = current->next;
		while (check)
		{
			if (current->arg == check->arg)
				return (1);
			check = check->next;
		}
		current = current->next;
	}
	return (0);
}

int	checker_parse_args(int argc, char **argv, t_list **a)
{
	t_list	*new_node;
	int		n;
	int		i;

	i = 1;
	while (i < argc)
	{
		if (!to_int(argv[i], &n))
			return (0);
		new_node = ft_lstnew(n);
		if (!new_node)
			return (0);
		ft_lstadd_back(a, new_node);
		i++;
	}
	if (duplicate(*a))
		return (0);
	return (1);
}

int	checker_sorted(t_list *a)
{
	while (a && a->next)
	{
		if (a->arg > a->next->arg)
			return (0);
		a = a->next;
	}
	return (1);
}
