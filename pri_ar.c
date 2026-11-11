#include "push_swap.h"

t_bool	chek_is_dgit(char *d)
{
	int	i;

	i = 0;
	if (d[i] == '-' || d[i] == '+')
		i++;
	while (d[i])
	{
		if (!(d[i] >= '0' && d[i] <= '9'))
			return (0);
		i++;
	}
	return (1);
}

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
	else if (ft_strcmp(argmnet, "--adaptive") == 0)
		flag = &list->adaptive;
	else if (ft_strcmp(argmnet, "--bench") == 0)
		flag = &list->bench;
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

static int	add_arg(char *arg, t_option *list, t_list **node, int *find)
{
	if (cheek_option(list, arg))
		return (1);
	if (!chek_is_dgit(arg))
		return (0);
	*node = creat_stack(arg, *node, *find);
	if (*node == NULL)
		return (0);
	*find = 1;
	return (1);
}

t_bool	cheek_is_arg_valid(char **argv, int argc, t_option *list,
			t_list *node)
{
	int	index;
	int	find;

	find = 0;
	index = 1;
	if (argc < 2)
		return (0);
	while (index < argc)
	{
		if (!add_arg(argv[index], list, &node, &find))
			return (0);
		index++;
	}
	if (find == 0)
		return (0);
	return (1);
}