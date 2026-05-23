/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   flood_fill.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kelle <kelle@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/23 02:41:30 by kelle             #+#    #+#             */
/*   Updated: 2026/05/23 02:42:57 by kelle            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

static bool	flood_fill(int x, int y, char **grid, int height)
{
	if (y < 0 || y >= height || !grid[y] || x < 0
		|| x >= (int)ft_strlen(grid[y]) || grid[y][x] == ' ')
		return (0);
	else if (grid[y][x] == '1' || grid[y][x] == 'f')
		return (2);
	grid[y][x] = 'f';
	if (!flood_fill(x + 1, y, grid, height))
		return (0);
	if (!flood_fill(x - 1, y, grid, height))
		return (0);
	if (!flood_fill(x, y + 1, grid, height))
		return (0);
	if (!flood_fill(x, y - 1, grid, height))
		return (0);
	return (1);
}

bool	flood_fill_prep(t_map *map)
{
	char	**copy;
	int		size;
	size_t	i;

	size = array_len(map->grid);
	copy = ft_calloc(size + 1, sizeof(char *));
	if (!copy)
		return (print_error(ALLOC_ERR), false);
	i = 0;
	while (map->grid[i])
	{
		copy[i] = ft_strdup(map->grid[i]);
		if (!copy[i])
			return (print_error(ALLOC_ERR), clear_matriz(copy), false);
		i++;
	}
	copy[i] = NULL;
	if (!flood_fill(map->start_x, map->start_y, copy, size))
		return (clear_matriz(copy), false);
	return (clear_matriz(copy), true);
}
