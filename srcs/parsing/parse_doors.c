/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_doors.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kelle <kelle@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/22 18:09:16 by kelle             #+#    #+#             */
/*   Updated: 2026/06/23 04:34:48 by kelle            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

static bool	check_door_position(char **grid, int y, int x)
{
	char	up;
	char	down;
	char	left;
	char	right;

	up = get_char_at(grid, y - 1, x);
	down = get_char_at(grid, y + 1, x);
	left = get_char_at(grid, y, x - 1);
	right = get_char_at(grid, y, x + 1);
	if (up == 'D' || down == 'D' || left == 'D' || right == 'D')
		return (false);
	if (left == '1' && right == '1' && is_walkable(up) && is_walkable(down))
		return (true);
	if (up == '1' && down == '1' && is_walkable(left) && is_walkable(right))
		return (true);
	return (false);
}

static int	count_doors(char **grid)
{
	int	i;
	int	j;
	int	count;

	i = 0;
	count = 0;
	while (grid[i])
	{
		j = 0;
		while (grid[i][j])
		{
			if (grid[i][j] == 'D')
				count++;
			j++;
		}
		i++;
	}
	return (count);
}

static bool	fill_doors(t_game *game)
{
	int	i;
	int	j;
	int	idx;

	i = 0;
	idx = 0;
	while (game->map->grid[i])
	{
		j = 0;
		while (game->map->grid[i][j])
		{
			if (game->map->grid[i][j] == 'D')
			{
				if (!check_door_position(game->map->grid, i, j))
					return (print_error("Invalid door position"), false);
				game->door[idx].x = j;
				game->door[idx].y = i;
				idx++;
			}
			j++;
		}
		i++;
	}
	return (true);
}

bool	check_doors(t_game *game)
{
	game->doors = count_doors(game->map->grid);
	if (game->doors == 0)
		return (true);
	game->door = ft_calloc(game->doors, sizeof(t_door));
	if (!game->door)
		return (print_error(ALLOC_ERR), false);
	return (fill_doors(game));
}
