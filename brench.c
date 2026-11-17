#include "push_swap.h"

static void	ft_print_disorder(t_stats *c)
{
	int	val;

	val = (int)(c->disorder * 10000 + 0.5f);
	ft_printf("%d.", val / 100);
	if (val % 100 < 10)
		ft_printf("0");
	ft_printf("%d%%\n", val % 100);
}

static void	ft_print_strategy(t_option *op)
{
	if (op->simple == 1)
		ft_printf("Simple O(n^2)\n");
	else if (op->medium == 1)
		ft_printf("Medium O(n*sqrt(n))\n");
	else if (op->complex == 1)
		ft_printf("Complex O(n log n)\n");
}

static void	ft_print_counts(t_stats *c)
{
	ft_printf("sa : %d\nsb : %d\nss : %d\n", c->sa, c->sb, c->ss);
	ft_printf("pa : %d\npb : %d\n", c->pa, c->pb);
	ft_printf("ra : %d\nrb : %d\nrr : %d\n", c->ra, c->rb, c->rr);
	ft_printf("rra : %d\nrrb : %d\nrrr : %d\n", c->rra, c->rrb, c->rrr);
}

void	ft_bench(t_stats *c, t_option *op)
{
	ft_print_disorder(c);
	ft_print_strategy(op);
	ft_printf("total number of operations : %d\n", c->total);
	ft_print_counts(c);
}