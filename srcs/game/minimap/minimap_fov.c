/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minimap_fov.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kelle <kelle@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/10 19:24:32 by kelle             #+#    #+#             */
/*   Updated: 2026/06/23 03:41:04 by kelle            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

static double	raycast_to_wall(t_game *game, t_dp start, t_dp dir,
		double max_distance)
{
	double	ray_x;
	double	ray_y;
	double	ray_step;
	double	ray_distance;

	ray_step = 0.05;
	ray_distance = 0.0;
	ray_x = start.x;
	ray_y = start.y;
	while (ray_distance < max_distance)
	{
		if (minimap_tile_color(game, ray_x, ray_y) == GREY
			|| minimap_tile_color(game, ray_x, ray_y) == MINIMAP_DOOR_COLOR)
			return (ray_distance);
		ray_x += dir.x * ray_step;
		ray_y += dir.y * ray_step;
		ray_distance += ray_step;
	}
	return (max_distance);
}

static t_dp	get_ray_end(t_game *game, int cx, int cy, t_dp dir)
{
	double	dist;
	t_dp	player;
	t_dp	end;

	player = (t_dp){game->player.player_x, game->player.player_y};
	dist = raycast_to_wall(game, player, dir, game->minimap_radius_tiles);
	dist *= game->minimap_tile_size;
	end.x = cx + dir.x * dist;
	end.y = cy + dir.y * dist;
	return (end);
}

static void	draw_single_fov_ray(t_game *game, int cx, int cy, t_fov_ray ray)
{
	double	ray_t;
	double	ray_len;
	t_dp	dir;

	ray_t = (double)ray.idx / (double)ray.total;
	dir.x = ray.left.x + (ray.right.x - ray.left.x) * ray_t;
	dir.y = ray.left.y + (ray.right.y - ray.left.y) * ray_t;
	ray_len = sqrt(dir.x * dir.x + dir.y * dir.y);
	if (ray_len > 0)
	{
		dir = (t_dp){dir.x / ray_len, dir.y / ray_len};
		draw_minimap_fov_line(game, (t_ip){cx, cy},
			get_ray_end(game, cx, cy, dir));
	}
}

static void	init_fov_dirs(t_game *game, t_dp *left, t_dp *right)
{
	left->x = game->player.dir_x - game->player.plane_x;
	left->y = game->player.dir_y - game->player.plane_y;
	right->x = game->player.dir_x + game->player.plane_x;
	right->y = game->player.dir_y + game->player.plane_y;
}

void	draw_minimap_fov(t_game *game, int center_x, int center_y)
{
	t_fov_ray	ray;
	int			rays;

	init_fov_dirs(game, &ray.left, &ray.right);
	rays = (int)(game->minimap_radius_tiles * game->minimap_tile_size * 5);
	if (rays < 24)
		rays = 24;
	ray.total = rays;
	ray.idx = 0;
	while (ray.idx <= rays)
	{
		draw_single_fov_ray(game, center_x, center_y, ray);
		ray.idx++;
	}
}
