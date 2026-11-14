#include "push_swap.h"

t_option *pre_option(t_option *o)
{
    o->simple = 0;
    o->medium = 0;
    o->complex = 0;
    o->adaptive = 0;
    o->bench = 0;
    return (o);
}

void	free_list(t_list *head)
{
	t_list	*next;

	while (head != NULL)
	{
		next = head->next;
		free(head);
		head = next;
	}
}
int main_porpes(int argc, char *argv[], t_list *list, t_option *op)
{
    t_stats     c;
    t_list      *stack_b;

    init_stats(&c);
    stack_b = NULL;
    if (cheek_is_arg_valid(argv, argc, pre_option(op), list))
    {
        if (op->simple == 1)
            ft_smeple(&list, &stack_b, &c);
        else if (op->medium == 1)
            ft_medimum(&list, &stack_b, ft_lstsize(list), &c);
        free_list(list);
        free_list(stack_b);
    }
    else
        return (free_list(list), write(2, "Error\n", 6), 0);
    return (1);
}
int main(int argc, char *argv[])
{
    t_option op;
    t_list   *list;


    if (argc >= 2)
    {
        list = malloc(sizeof(t_list));
        if (list == NULL)
            return(0);
        list->next = NULL;
       main_porpes(argc, argv, list, &op);
    }
    else
        return (0);
    return (1);
}