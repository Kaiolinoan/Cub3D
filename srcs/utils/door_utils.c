/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   door_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kelle <kelle@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/23 03:47:52 by kelle             #+#    #+#             */
/*   Updated: 2026/06/23 04:36:34 by kelle            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

t_door	*get_door_at(t_game *game, int x, int y)
{
	int	i;

	i = 0;
	while (i < game->doors)
	{
		if (game->door[i].x == x && game->door[i].y == y)
			return (&game->door[i]);
		i++;
	}
	return (NULL);
}

char	get_char_at(char **grid, int y, int x)
{
	if (y < 0 || !grid[y] || x < 0 || x >= (int)ft_strlen(grid[y]))
		return (' ');
	return (grid[y][x]);
}

bool	is_walkable(char c)
{
	return (c == '0' || c == 'N' || c == 'S' || c == 'W' || c == 'E');
}
