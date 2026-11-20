
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
	limit = (sign == -1) ? (long long)INT_MAX + 1 : INT_MAX;
	n = 0;
	if (s[i] == '\0')
		return (0);
	while (s[i])
	{
		if (s[i] < '0' || s[i] > '9')
			return (0);
		if (n > (limit - (s[i] - '0')) / 10)
			return (0);
		n = n * 10 + s[i++] - '0';
	}
	*value = (int)(n * sign);
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

void	checker_free(t_list *stack)
{
	t_list	*next;

	while (stack)
	{
		next = stack->next;
		free(stack);
		stack = next;
	}
}

static void	trim_newline(char *line)
{
	int	i;

	i = 0;
	while (line[i] && line[i] != '\n')
		i++;
	if (line[i] == '\n')
		line[i] = '\0';
}

static int	read_instructions(t_list **a, t_list **b)
{
	char	*line;
	int		error;

	error = 0;
	while ((line = get_next_line(0)) != NULL)
	{
		trim_newline(line);
		if (!error && !checker_execute(line, a, b))
			error = 1;
		free(line);
	}
	return (error);
}

static void	print_result(int error, t_list *a, t_list *b)
{
	if (error)
		write(2, "Error\n", 6);
	else if (checker_sorted(a) && b == NULL)
		write(1, "OK\n", 3);
	else
		write(1, "KO\n", 3);
}

int	main(int argc, char **argv)
{
	t_list	*a;
	t_list	*b;
	int		error;

	if (argc == 1)
		return (0);
	a = NULL;
	b = NULL;
	error = !checker_parse_args(argc, argv, &a);
	if (!error)
		error = read_instructions(&a, &b);
	print_result(error, a, b);
	checker_free(a);
	checker_free(b);
	return (0);
}