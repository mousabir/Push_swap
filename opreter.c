#include "push_swap.h"

void	init_stats(t_stats *c)
{
	c->size = 0;
	c->total = 0;
	c->sa = 0;
	c->sb = 0;
	c->ss = 0;
	c->pa = 0;
	c->pb = 0;
	c->ra = 0;
	c->rb = 0;
	c->rr = 0;
	c->rra = 0;
	c->rrb = 0;
	c->rrr = 0;
}

void	count_move(t_stats *c, char *an)
{
	if (ft_strcmp(an, "sa") == 0)
		c->sa++;
	else if (ft_strcmp(an, "sb") == 0)
		c->sb++;
	else if (ft_strcmp(an, "ss") == 0)
		c->ss++;
	else if (ft_strcmp(an, "pa") == 0)
		c->pa++;
	else if (ft_strcmp(an, "pb") == 0)
		c->pb++;
	else if (ft_strcmp(an, "ra") == 0)
		c->ra++;
	else if (ft_strcmp(an, "rb") == 0)
		c->rb++;
	else if (ft_strcmp(an, "rr") == 0)
		c->rr++;
	else if (ft_strcmp(an, "rra") == 0)
		c->rra++;
	else if (ft_strcmp(an, "rrb") == 0)
		c->rrb++;
	else if (ft_strcmp(an, "rrr") == 0)
		c->rrr++;
	c->total++;
}