/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klino-an <klino-an@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/23 02:51:13 by kelle             #+#    #+#             */
/*   Updated: 2026/06/01 05:52:27 by kelle            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

void	print_error(char *msg)
{
	char	*header;

	header = "\033[1;31mError\033[0m\n";
	ft_dprintf(2, "%s%s\n", header, msg);
}

size_t	array_len(char **array)
{
	size_t	i;

	i = 0;
	if (!array)
		return (0);
	while (array[i])
		i++;
	return (i);
}

void	remove_new_line_in_array(char **arr)
{
	size_t	i;
	size_t	j;

	i = 0;
	while (arr[i])
	{
		j = 0;
		while (arr[i][j])
		{
			if (arr[i][j] == '\n')
				arr[i][j] = '\0';
			j++;
		}
		i++;
	}
}

void	remove_new_line(char *str)
{
	size_t	i;

	i = 0;
	while (str[i])
	{
		if (str[i] == '\n')
			str[i] = '\0';
		i++;
	}
}

double	ft_abs(double num)
{
	if (num < 0)
		num = num * -1;
	return (num);
}
