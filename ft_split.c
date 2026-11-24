/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_split.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ilazerou <ilazerou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/24 21:36:56 by ilazerou          #+#    #+#             */
/*   Updated: 2026/11/24 21:38:06 by ilazerou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>

static int	count_words(char *str, char c)
{
	int	i;
	int	conter;

	conter = 0;
	i = 0;
	while (str[i])
	{
		while (str[i] == c)
			i++;
		if (str[i] == '\0')
			return (conter);
		conter++;
		while (str[i] != c && str[i] != '\0')
			i++;
	}
	return (conter);
}

static char	*git_word(char *str, int len)
{
	char	*s;
	int		i;

	i = 0;
	s = malloc((len + 1) * sizeof(char));
	if (s == NULL)
		return (NULL);
	while (i < len)
	{
		s[i] = str[i];
		i++;
	}
	s[i] = '\0';
	return (s);
}

static int	free_all(char **s, int n)
{
	while (n > 0)
	{
		n--;
		free(s[n]);
	}
	free(s);
	return (0);
}

static int	cpy_words(char **s, char *str, char c)
{
	int	j;
	int	index;
	int	st;

	j = 0;
	index = 0;
	while (str[j])
	{
		while (str[j] == c)
			j++;
		if (str[j] == '\0')
			break ;
		st = j;
		while (str[j] && str[j] != c)
			j++;
		s[index] = git_word(&str[st], j - st);
		if (s[index] == NULL)
			return (free_all(s, index));
		index++;
	}
	s[index] = NULL;
	return (1);
}

char	**ft_split(char *str, char c)
{
	char	**s;

	if (str == NULL)
		return (NULL);
	s = malloc((count_words(str, c) + 1) * sizeof(char *));
	if (s == NULL)
		return (NULL);
	if (cpy_words(s, str, c) == 0)
		return (NULL);
	return (s);
}
