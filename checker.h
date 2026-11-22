/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   checker.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mousabir <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/22 17:26:02 by mousabir          #+#    #+#             */
/*   Updated: 2026/11/22 17:32:35 by mousabir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHECKER_H
# define CHECKER_H

# include "push_swap.h"

char	*get_next_line(int fd);
int		checker_parse_args(int argc, char **argv, t_list **a);
int		checker_execute(char *line, t_list **a, t_list **b);
int		checker_sorted(t_list *a);
void	checker_free(t_list *stack);
int		execute_push_swap(char *line, t_list **a, t_list **b);
void	swap_stack(t_list *stack);

#endif