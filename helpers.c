#include "push_swap.h"

t_list	*ft_lstnew(int arg)
{
	t_list	*now_node;

	now_node = malloc(sizeof(t_list));
	if (now_node == NULL)
		return (NULL);
	now_node->arg = arg;
	now_node->next = NULL;
	return (now_node);
}

int	ft_strcmp(const char *s1, const char *s2)
{
	size_t	i;

	i = 0;
	while (s1[i] && s1[i] == s2[i])
		i++;
	return ((unsigned char)s1[i] - (unsigned char)s2[i]);
}

void	ft_lstadd_back(t_list **lst, t_list *new)
{
	t_list	*use_me;

	if (lst == NULL || new == NULL)
		return ;
	if (*lst == NULL)
	{
		*lst = new;
		return ;
	}
	use_me = *lst;
	while (use_me->next != NULL)
		use_me = use_me->next;
	use_me->next = new;
}

int	ft_atoi(const char *nptr)
{
	int	i;
	int	sgnd;
	int	result;

	i = 0;
	sgnd = 1;
	result = 0;
	while (nptr[i] == 32 || (nptr[i] >= 9 && nptr[i] <= 13))
		i++;
	if (nptr[i] == '-' || nptr[i] == '+')
	{
		if (nptr[i] == '-')
			sgnd *= -1;
		i++;
	}
	while (nptr[i] >= '0' && nptr[i] <= '9')
	{
		result = result * 10 + (nptr[i] - 48);
		i++;
	}
	return (result * sgnd);
}

void	ft_lstadd_front(t_list **lst, t_list *new)
{
	if (lst == NULL || new == NULL)
		return ;
	new->next = *lst;
	*lst = new;
}