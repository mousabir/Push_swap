/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   checker_bonus.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ilazerou <ilazerou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/22 17:18:50 by mousabir          #+#    #+#             */
/*   Updated: 2026/11/24 21:36:30 by ilazerou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "checker.h"

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
	line = get_next_line(0);
	while (line != NULL)
	{
		trim_newline(line);
		if (!error && !checker_execute(line, a, b))
			error = 1;
		free(line);
		line = get_next_line(0);
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
	t_list		*a;
	t_list		*b;
	t_option	o;
	int			error;

	if (argc == 1)
		return (0);
	a = malloc(sizeof(t_list));
	if (a == NULL)
		return (1);
	b = NULL;
	error = !check_args(argv, argc, &o, a);
	if (!error)
		error = read_instructions(&a, &b);
	print_result(error, a, b);
	checker_free(a);
	checker_free(b);
	return (0);
}
