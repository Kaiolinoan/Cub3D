/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kelle <kelle@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/10 19:16:38 by kelle             #+#    #+#             */
/*   Updated: 2026/06/23 03:39:09 by kelle            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

void	display_minimap(t_game *game)
{
	if (game->minimap == false)
		game->minimap = true;
	else
		game->minimap = false;
}

int	minimap_map_height(t_map *map)
{
	return ((int)array_len(map->grid));
}

unsigned int	rgb_to_color(t_rgb *rgb)
{
	return ((rgb->r << 16) | (rgb->g << 8) | rgb->b);
}

void	put_minimap_pixel(t_game *game, int x, int y, unsigned int color)
{
	if (x < 0 || y < 0 || x >= game->win_w || y >= game->win_h)
		return ;
	my_pixel_put(game, x, y, color);
}

unsigned int	minimap_tile_color(t_game *game, double world_x, double world_y)
{
	int		cell_x;
	int		cell_y;
	char	cell;
	int		map_height;

	map_height = minimap_map_height(game->map);
	cell_x = (int)floor(world_x);
	cell_y = (int)floor(world_y);
	if (cell_y < 0 || cell_y >= map_height || !game->map->grid[cell_y])
		return (rgb_to_color(game->map->ceiling));
	if (cell_x < 0 || cell_x >= (int)ft_strlen(game->map->grid[cell_y]))
		return (rgb_to_color(game->map->ceiling));
	cell = game->map->grid[cell_y][cell_x];
	if (cell == '1')
		return (GREY);
	if (cell == ' ')
		return (rgb_to_color(game->map->ceiling));
	if (cell == 'D')
		return (MINIMAP_DOOR_COLOR);
	return (rgb_to_color(game->map->floor));
}
