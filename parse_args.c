/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_args.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ilazerou <ilazerou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/22 17:22:35 by mousabir          #+#    #+#             */
/*   Updated: 2026/11/24 21:38:48 by ilazerou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

t_bool	cheek_option(t_option *list, char *argmnet)
{
	t_bool	*flag;

	flag = NULL;
	if (ft_strcmp(argmnet, "--simple") == 0)
		flag = &list->simple;
	else if (ft_strcmp(argmnet, "--medium") == 0)
		flag = &list->medium;
	else if (ft_strcmp(argmnet, "--complex") == 0)
		flag = &list->complex;
	else if (ft_strcmp(argmnet, "--bench") == 0)
		flag = &list->bench;
	else if (ft_strcmp(argmnet, "--adaptive") == 0)
		flag = &list->adaptive;
	if (flag == NULL)
		return (0);
	*flag = 1;
	return (1);
}

t_list	*creat_stack(char *argv, t_list *node, int find)
{
	if (find == 0)
	{
		node->arg = ft_atoi(argv);
		return (node);
	}
	else
	{
		node->next = ft_lstnew(ft_atoi(argv));
		node = node->next;
		return (node);
	}
}

t_bool	chek_is_dgit(char *d)
{
	int	i;

	i = 0;
	if (d[i] == '-' || d[i] == '+')
		i++;
	if (d[i] == '\0')
		return (0);
	while (d[i])
	{
		if (!(d[i] >= '0' && d[i] <= '9'))
			return (0);
		i++;
	}
	return (1);
}

t_bool	check_args(char **argv, int argc, t_option *list, t_list *node)
{
	int		index;
	int		find;
	t_list	*head;

	find = 0;
	index = 1;
	head = node;
	if (argc < 2)
		return (0);
	while (index < argc)
	{
		if (!parse_one_arg(argv[index], list, &node, &find))
			return (0);
		index++;
	}
	if (find == 0 || has_dup(head))
		return (0);
	return (1);
}
