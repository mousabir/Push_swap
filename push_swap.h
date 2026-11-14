#ifndef PUSH_SWAP_H
# define PUSH_SWAP_H

# include "ft_printf.h"
# include <stdlib.h>
# include <stdio.h>
# include <stddef.h>
#include <unistd.h>
#include <limits.h>

typedef struct s_stats
{
	long	size;
	long	total;
	long	sa;
	long	sb;
	long	ss;
	long	pa;
	long	pb;
	long	ra;
	long	rb;
	long	rr;
	long	rra;
	long	rrb;
	long	rrr;
}	t_stats;

typedef struct s_list
{
	int				arg;
	int 			index;
	struct s_list	*next;
}	t_list;

typedef unsigned int	t_bool;

typedef struct s_option
{
	t_bool	simple;
	t_bool	medium;
	t_bool	complex;
	t_bool	adaptive;
	t_bool	bench;
}	t_option;

void sort_array(int *array, int size);
void asind_index(int *array, t_list *head, int size);
void creat_rank(t_list *head);
void	ft_lstadd_front(t_list **lst, t_list *new);
void	ft_lstadd_back(t_list **lst, t_list *new);
long	smallest_position(t_list *head);
t_list	*ft_lstnew(int arg);
int		ft_strcmp(const char *s1, const char *s2);
t_bool	cheek_is_arg_valid(char **argv, int argc, t_option *list, t_list *node);
t_bool	chek_is_dgit(char *d);
t_list    *creat_stack(char *argv, t_list *node, int find);
int		ft_atoi(const char *nptr);
t_bool	cheek_option(t_option *list, char *argmnet);
void	bring_to_top(long pos, t_list **a, t_list **b, t_stats *c);
t_list	*find_node(t_list *stack, long pos);
long	ft_lstsize(t_list *lst);
void	move_a(char *an, t_list **a, t_list **b, t_stats *c);
void	rotate_times(char *an, long times, t_list **a, t_stats *c);
void	push_node(t_list **a, t_list **b);
void	rotate_node(t_list **a);
void	reverse_node(t_list **a, long n);
void	ft_smeple(t_list **head, t_list **list_b, t_stats *c);
void	count_move(t_stats *c, char *an);
void	init_stats(t_stats *c);
void	free_list(t_list *head);
t_option *pre_option(t_option *o);
int ft_sqrt(int nb);
void ft_medimum(t_list **head, t_list **stack_b, long size, t_stats *c);
t_bool	chek_range(char *s);
t_bool	has_dup(t_list *head);

#endif