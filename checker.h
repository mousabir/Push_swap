
#ifndef CHECKER_H
# define CHECKER_H

# include "push_swap.h"

char	*get_next_line(int fd);
int		checker_parse_args(int argc, char **argv, t_list **a);
int		checker_execute(char *line, t_list **a, t_list **b);
int		checker_sorted(t_list *a);
void	checker_free(t_list *stack);

#endif