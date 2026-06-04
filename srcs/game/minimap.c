/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minimap2.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kelle <kelle@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/27 03:26:05 by kelle             #+#    #+#             */
/*   Updated: 2026/06/03 21:03:18 by kelle            ###   ########.fr       */
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

static int	minimap_map_height(t_map *map)
{
	return ((int)array_len(map->grid));
}

unsigned int	rgb_to_color(t_rgb *rgb)
{
	return ((rgb->r << 16) | (rgb->g << 8) | rgb->b);
}

static unsigned int	minimap_tile_color(t_game *game, double world_x,
	double world_y)
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
	return (rgb_to_color(game->map->floor));
}

static void	put_minimap_pixel(t_game *game, int x, int y, unsigned int color)
{
	if (x < 0 || y < 0 || x >= game->win_w || y >= game->win_h)
		return ;
	my_pixel_put(game, x, y, color);
}

static unsigned int	blend_color_with_white(unsigned int base_color, double white_opacity)
{
	unsigned char	r;
	unsigned char	g;
	unsigned char	b;

	r = (base_color >> 16) & 0xFF;
	g = (base_color >> 8) & 0xFF;
	b = base_color & 0xFF;
	r = (unsigned char)(r + (255 - r) * white_opacity);
	g = (unsigned char)(g + (255 - g) * white_opacity);
	b = (unsigned char)(b + (255 - b) * white_opacity);
	return ((r << 16) | (g << 8) | b);
}

static bool	minimap_in_circle(int x, int y, int center_x, int center_y, int radius_px)
{
	int	center_offset_x;
	int	center_offset_y;

	center_offset_x = x - center_x;
	center_offset_y = y - center_y;
	return (center_offset_x * center_offset_x + center_offset_y * center_offset_y <= radius_px * radius_px);
}

static double	raycast_to_wall(t_game *game, double start_x, double start_y, double dir_x, double dir_y, double max_distance)
{
	double	ray_x;
	double	ray_y;
	double	ray_step;
	double	ray_distance;

	ray_step = 0.05;
	ray_distance = 0.0;
	ray_x = start_x;
	ray_y = start_y;
	while (ray_distance < max_distance)
	{
		if (minimap_tile_color(game, ray_x, ray_y) == GREY)
			return (ray_distance);
		ray_x += dir_x * ray_step;
		ray_y += dir_y * ray_step;
		ray_distance += ray_step;
	}
	return (max_distance);
}

static void	draw_minimap_fov_line(t_game *game, int start_x, int start_y, double end_x, double end_y)
{
	double			line_step_x;
	double			line_step_y;
	double			line_x;
	double			line_y;
	int				steps;
	int				line_index;
	int				pixel_x;
	int				pixel_y;
	int				radius_px;
	unsigned int	fov_color;

	line_step_x = end_x - start_x;
	line_step_y = end_y - start_y;
	if (ft_abs(line_step_x) > ft_abs(line_step_y))
		steps = ft_abs(line_step_x);
	else
		steps = ft_abs(line_step_y);
	if (steps <= 0)
		steps = 1;
	line_x = start_x;
	line_y = start_y;
	radius_px = (int)(game->minimap_radius_tiles * game->minimap_tile_size);
	fov_color = blend_color_with_white(rgb_to_color(game->map->floor), 0.2);
	line_index = 0;
	while (line_index <= steps)
	{
		pixel_x = (int)(line_x + 0.5);
		pixel_y = (int)(line_y + 0.5);
		if (minimap_in_circle(pixel_x, pixel_y, start_x, start_y, radius_px))
			put_minimap_pixel(game, pixel_x, pixel_y, fov_color);
		line_x += line_step_x / steps;
		line_y += line_step_y / steps;
		line_index++;
	}
}

static void	draw_minimap_fov(t_game *game, int center_x, int center_y)
{
	double	fov_left_x;
	double	fov_left_y;
	double	fov_right_x;
	double	fov_right_y;
	double	ray_dir_x;
	double	ray_dir_y;
	double	ray_dir_len;
	double	ray_t;
	int		rays;
	int		ray_index;
	double	radius_px;
	double	radius_world;
	double	distance_to_wall;
	double	pixel_distance;
	double	endpoint_x;
	double	endpoint_y;

	radius_px = game->minimap_radius_tiles * game->minimap_tile_size;
	radius_world = game->minimap_radius_tiles;
	fov_left_x = game->player.dir_x - game->player.plane_x;
	fov_left_y = game->player.dir_y - game->player.plane_y;
	fov_right_x = game->player.dir_x + game->player.plane_x;
	fov_right_y = game->player.dir_y + game->player.plane_y;
	rays = (int)(radius_px * 5);
	if (rays < 24)
		rays = 24;
	ray_index = 0;
	while (ray_index <= rays)
	{
		ray_t = (double)ray_index / (double)rays;
		ray_dir_x = fov_left_x + (fov_right_x - fov_left_x) * ray_t;
		ray_dir_y = fov_left_y + (fov_right_y - fov_left_y) * ray_t;
		ray_dir_len = sqrt(ray_dir_x * ray_dir_x + ray_dir_y * ray_dir_y);
		if (ray_dir_len > 0)
		{
			distance_to_wall = raycast_to_wall(game, game->player.player_x, game->player.player_y, ray_dir_x / ray_dir_len, ray_dir_y / ray_dir_len, radius_world);
			pixel_distance = distance_to_wall * game->minimap_tile_size;
			if (pixel_distance > radius_px)
				pixel_distance = radius_px;
			endpoint_x = center_x + (ray_dir_x / ray_dir_len) * pixel_distance;
			endpoint_y = center_y + (ray_dir_y / ray_dir_len) * pixel_distance;
			draw_minimap_fov_line(game, center_x, center_y, endpoint_x, endpoint_y);
		}
		ray_index++;
	}
}

static void	draw_minimap_player(t_game *game, int center_x, int center_y)
{
	int		marker_offset_x;
	int		marker_offset_y;
	double	radius;
	double	radius_sq;

	radius = game->minimap_tile_size * 0.3;
	radius_sq = radius * radius;
	marker_offset_y = -(int)radius;
	while (marker_offset_y <= (int)radius)
	{
		marker_offset_x = -(int)radius;
		while (marker_offset_x <= (int)radius)
		{
			if ((marker_offset_x * marker_offset_x + marker_offset_y * marker_offset_y) <= radius_sq)
				put_minimap_pixel(game, center_x + marker_offset_x, center_y + marker_offset_y, MINIMAP_PLAYER_COLOR);
			marker_offset_x++;
		}
		marker_offset_y++;
	}
}

void	render_minimap(t_game *game)
{
	int		radius_px;
	int		diameter;
	int		offset_x;
	int		offset_y;
	int		pixel_x;
	int		pixel_y;
	double	pixel_offset_x;
	double	pixel_offset_y;
	double	world_x;
	double	world_y;
	double	player_world_x;
	double	player_world_y;

	radius_px = (int)(game->minimap_radius_tiles * game->minimap_tile_size);
	diameter = radius_px * 2;
	offset_x = MINIMAP_MARGIN;
	offset_y = MINIMAP_MARGIN;
	player_world_x = game->player.player_x;
	player_world_y = game->player.player_y;
	pixel_y = 0;
	while (pixel_y < diameter)
	{
		pixel_x = 0;
		while (pixel_x < diameter)
		{
			pixel_offset_x = pixel_x - radius_px;
			pixel_offset_y = pixel_y - radius_px;
			if (pixel_offset_x * pixel_offset_x + pixel_offset_y * pixel_offset_y <= (double)(radius_px * radius_px))
			{
				world_x = player_world_x + (pixel_offset_x / (double)game->minimap_tile_size);
				world_y = player_world_y + (pixel_offset_y / (double)game->minimap_tile_size);
				put_minimap_pixel(game, offset_x + pixel_x, offset_y + pixel_y, minimap_tile_color(game, world_x, world_y));
			}
			pixel_x++;
		}
		pixel_y++;
	}
	draw_minimap_fov(game, offset_x + radius_px, offset_y + radius_px);
	pixel_y = 0;
	while (pixel_y < diameter)
	{
		pixel_x = 0;
		while (pixel_x < diameter)
		{
			pixel_offset_x = pixel_x - radius_px;
			pixel_offset_y = pixel_y - radius_px;
			if (pixel_offset_x * pixel_offset_x + pixel_offset_y * pixel_offset_y <= (double)(radius_px * radius_px)
				&& pixel_offset_x * pixel_offset_x + pixel_offset_y * pixel_offset_y >= (double)((radius_px - 2)
					* (radius_px - 2)))
				put_minimap_pixel(game, offset_x + pixel_x, offset_y + pixel_y, 0x00555555);
			pixel_x++;
		}
		pixel_y++;
	}
	draw_minimap_player(game, offset_x + radius_px, offset_y + radius_px);
}
