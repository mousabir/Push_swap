/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_dprintf.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mousabir <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/22 17:21:18 by mousabir          #+#    #+#             */
/*   Updated: 2026/11/22 17:21:28 by mousabir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

static int	ft_dputchar(char c)
{
	return (write(2, &c, 1));
}

static int	ft_dputnbr(int n)
{
	long	br;
	int		count;

	br = n;
	count = 0;
	if (br < 0)
	{
		count += ft_dputchar('-');
		br = -br;
	}
	if (br >= 10)
		ft_dputnbr(br / 10);
	count += ft_dputchar((br % 10) + '0');
	return (count);
}

static int	chek_dformte(char c, va_list *traker)
{
	if (c == 's')
		return (ft_putstr(va_arg(*traker, char *)));
	if (c == 'd' || c == 'i')
		return (ft_dputnbr(va_arg(*traker, int)));
	if (c == 'c')
		return (ft_putchar(va_arg(*traker, int)));
	if (c == 'x')
		return (ft_put_hex(va_arg(*traker, unsigned int), "0123456789abcdef"));
	if (c == 'X')
		return (ft_put_hex(va_arg(*traker, unsigned int), "0123456789ABCDEF"));
	if (c == 'p')
		return (ft_put_adrss(va_arg(*traker, void *)));
	if (c == 'u')
		return (ft_put_unsigned(va_arg(*traker, unsigned int)));
	return (ft_dputchar(c));
}

int	ft_dprintf(const char *formte, ...)
{
	va_list	traker;
	int		i;
	int		count;

	i = 0;
	count = 0;
	va_start(traker, formte);
	while (formte[i])
	{
		if (formte[i] == '%' && formte[i + 1])
		{
			i++;
			count += chek_dformte(formte[i], &traker);
		}
		else if (formte[i] != '%')
			count += ft_dputchar(formte[i]);
		i++;
	}
	va_end(traker);
	return (count);
}
