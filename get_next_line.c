/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ilazerou <ilazerou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/20 16:09:06 by ilazerou          #+#    #+#             */
/*   Updated: 2026/11/20 16:10:31 by ilazerou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"

char	*get_str(int fd, char *stach)
{
	char	*tmp;
	char	*str;
	int		n;

	n = 1;
	while (ft_strchr(stach, '\n') == NULL && n != 0)
	{
		tmp = malloc(sizeof(char) * BUFFER_SIZE + 1);
		if (!tmp)
			return (free(stach), NULL);
		n = read(fd, tmp, BUFFER_SIZE);
		if (n == -1)
			return (free(stach), free(tmp), NULL);
		tmp[n] = '\0';
		str = ft_strjoin(stach, tmp);
		free(tmp);
		if (!str)
			return (free(stach), NULL);
		free(stach);
		stach = str;
	}
	return (stach);
}

char	*return_str(char *stach)
{
	char	*buff;
	ssize_t	i;

	i = 0;
	while (stach[i] && stach[i] != '\n')
		i++;
	buff = malloc(i + 2);
	if (!buff)
		return (NULL);
	i = 0;
	while (stach[i] && stach[i] != '\n')
	{
		buff[i] = stach[i];
		i++;
	}
	if (stach[i] == '\n')
	{
		buff[i] = '\n';
		i++;
	}
	buff[i] = '\0';
	return (buff);
}

char	*get_next_str(char *stach)
{
	char	*buff;
	ssize_t	j;
	ssize_t	i;

	i = 0;
	while (stach[i] && stach[i] != '\n')
		i++;
	if (stach[i] == '\0')
		return (free(stach), NULL);
	i++;
	buff = malloc(ft_strlen(stach) - i + 1);
	if (!buff)
		return (NULL);
	j = 0;
	while (stach[i])
	{
		buff[j] = stach[i];
		i++;
		j++;
	}
	buff[j] = '\0';
	free(stach);
	return (buff);
}

char	*get_next_line(int fd)
{
	static char	*stach;
	char		*str;

	if (fd < 0 || BUFFER_SIZE <= 0)
		return (NULL);
	if (!stach)
	{
		stach = malloc(sizeof(char) * BUFFER_SIZE + 1);
		if (!stach)
			return (NULL);
		stach[0] = '\0';
	}
	stach = get_str(fd, stach);
	if (!stach || stach[0] == '\0')
	{
		free(stach);
		stach = NULL;
		return (NULL);
	}
	str = return_str(stach);
	stach = get_next_str(stach);
	return (str);
}
