/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_args_utiles.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ilazerou <ilazerou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/22 17:22:48 by mousabir          #+#    #+#             */
/*   Updated: 2026/11/24 21:39:22 by ilazerou         ###   ########.fr       */
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

static void	free_split(char **words)
{
	int	i;

	i = 0;
	while (words[i])
	{
		free(words[i]);
		i++;
	}
	free(words);
}

static int	add_arg(char *arg, t_option *list, t_list **node, int *find)
{
	if (cheek_option(list, arg))
		return (1);
	if (!chek_is_dgit(arg) || !chek_range(arg))
		return (0);
	*node = creat_stack(arg, *node, *find);
	if (*node == NULL)
		return (0);
	*find = 1;
	return (1);
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

t_bool	parse_one_arg(char *arg, t_option *list, t_list **node, int *find)
{
	char	**words;
	int		i;

	words = ft_split(arg, ' ');
	if (!words)
		return (0);
	if (!words[0])
	{
		free_split(words);
		return (0);
	}
	i = 0;
	while (words[i])
	{
		if (!add_arg(words[i], list, node, find))
		{
			free_split(words);
			return (0);
		}
		i++;
	}
	free_split(words);
	return (1);
}
