/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kelle <kelle@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/23 02:49:20 by kelle             #+#    #+#             */
/*   Updated: 2026/05/23 02:50:57 by kelle            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

bool	check_full_line(char *full_line)
{
	int	i;

	i = 0;
	if (!full_line)
		return (false);
	while (full_line[i])
	{
		if (full_line[i] != '\n')
			return (true);
		i++;
	}
	return (false);
}

bool	check_empty_line_on_map(char *full_line)
{
	int		i;
	bool	flag;

	i = 0;
	flag = 0;
	if (!full_line)
		return (false);
	while (full_line[i])
	{
		if (full_line[i] != '\n')
			flag = 1;
		if (flag && full_line[i] == '\n' && full_line[i + 1] == '\n')
			return (false);
		i++;
	}
	return (true);
}

char	*ft_strjoin_and_free(char *s1, char *s2)
{
	char	*str;
	size_t	i;
	size_t	j;

	str = ft_calloc(1, ft_strlen(s1) + ft_strlen(s2) + 1);
	if (!str)
		return (print_error(ALLOC_ERR), NULL);
	i = 0;
	j = 0;
	while (s1 && s1[i])
	{
		str[i] = s1[i];
		i++;
	}
	while (s2 && s2[j])
		str[i++] = s2[j++];
	return (str[i] = '\0', free(s1), str);
}

int	ft_strcmp(char *s1, char *s2)
{
	if (!s1 || !s2)
		return (-1);
	while (*s1 && *s1 == *s2)
	{
		s1++;
		s2++;
	}
	return ((unsigned char)*s1 - (unsigned char)*s2);
}

bool	check_extension(char *filename, char *extension)
{
	char	*last_dot;

	last_dot = ft_strrchr(filename, '.');
	if (!last_dot)
		return (false);
	remove_new_line(last_dot);
	if (!ft_strcmp(last_dot, extension))
		return (true);
	return (false);
}
