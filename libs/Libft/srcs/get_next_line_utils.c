/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line_utils.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kemontei <kemontei@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/07 16:15:46 by kemontei          #+#    #+#             */
/*   Updated: 2025/05/19 18:18:24 by kemontei         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"

size_t	gnl_strlen(const char *str)
{
	size_t	i;

	i = 0;
	if (!str)
		return (0);
	while (str[i])
	{
		if (str[i++] == '\n')
			break ;
	}
	return (i);
}

char	*gnl_strjoin(char *s1, char *s2)
{
	char	*str;
	size_t	i;
	size_t	j;

	str = malloc(gnl_strlen(s1) + gnl_strlen(s2) + 1);
	if (!str)
		return (NULL);
	i = 0;
	while (s1 && s1[i])
	{
		str[i] = s1[i];
		i++;
	}
	j = 0;
	while (s2[j])
	{
		str[i++] = s2[j];
		if (s2[j++] == '\n')
			break ;
	}
	str[i] = '\0';
	free (s1);
	return (str);
}

char	*gnl_strchr(const char *str, int c)
{
	size_t	i;

	i = 0;
	if (str)
	{
		while (str[i])
		{
			if (str[i] == (char)c)
				return ((char *)&str[i]);
			i++;
		}
		if ((char)c == '\0')
			return ((char *)&str[i]);
	}
	return (NULL);
}

void	update_buffer(char *buffer)
{
	size_t	i;
	size_t	j;

	i = 0;
	j = 0;
	while (buffer[i] && buffer[i] != '\n')
		i++;
	if (buffer[i] == '\n')
		i++;
	while (buffer[i])
		buffer[j++] = buffer[i++];
	buffer[j] = '\0';
}
