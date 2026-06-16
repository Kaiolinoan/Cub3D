/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   raycasting.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kelle <kelle@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/10 19:12:09 by kelle             #+#    #+#             */
/*   Updated: 2026/06/10 19:12:10 by kelle            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

static void	cast_ray(t_game *game, t_ray *ray)
{
	int	x;
	int	y;

	while (!ray->hit)
	{
		if (ray->side_dist_x < ray->side_dist_y)
		{
			ray->side_dist_x += ray->delta_dist_x;
			ray->map_x += ray->step_x;
			ray->side = 0;
		}
		else
		{
			ray->side_dist_y += ray->delta_dist_y;
			ray->map_y += ray->step_y;
			ray->side = 1;
		}
		x = ray->map_x;
		y = ray->map_y;
		if (y < 0 || y >= game->win_h
			|| x < 0 || x >= (int)strlen(game->map->grid[ray->map_y]))
			return ;
		if (game->map->grid[ray->map_y][ray->map_x] == '1')
			ray->hit = 1;
	}
}

static void	check_raydir_x(t_player *player, t_ray *ray)
{
	if (ray->ray_dir_x == 0)
		ray->delta_dist_x = 1e30;
	else
		ray->delta_dist_x = ft_abs(1.0 / ray->ray_dir_x);
	if (ray->ray_dir_x < 0)
	{
		ray->step_x = -1;
		ray->side_dist_x = (player->player_x - ray->map_x)
			* ray->delta_dist_x;
	}
	else
	{
		ray->step_x = 1;
		ray->side_dist_x = (ray->map_x + 1.0 - player->player_x)
			* ray->delta_dist_x;
	}
}

static void	check_raydir_y(t_player *player, t_ray *ray)
{
	if (ray->ray_dir_y == 0)
		ray->delta_dist_y = 1e30;
	else
		ray->delta_dist_y = ft_abs(1.0 / ray->ray_dir_y);
	if (ray->ray_dir_y < 0)
	{
		ray->step_y = -1;
		ray->side_dist_y = (player->player_y - ray->map_y)
			* ray->delta_dist_y;
	}
	else
	{
		ray->step_y = 1;
		ray->side_dist_y = (ray->map_y + 1.0 - player->player_y)
			* ray->delta_dist_y;
	}
}

static void	init_ray(t_game *game, t_player *player, t_ray *ray, int x)
{
	ray->camera_x = 2 * x / (double)game->win_w - 1;
	ray->ray_dir_x = player->dir_x + player->plane_x * ray->camera_x;
	ray->ray_dir_y = player->dir_y + player->plane_y * ray->camera_x;
	ray->map_x = (int) player->player_x;
	ray->map_y = (int) player->player_y;
	ray->hit = false;
	check_raydir_x(player, ray);
	check_raydir_y(player, ray);
}

int	raycasting(t_game *game)
{
	t_ray	ray;
	int		x;

	x = 0;
	while (x < game->win_w)
	{
		init_ray(game, &game->player, &ray, x);
		cast_ray(game, &ray);
		calculate_wall(game, &ray);
		draw_textured_line(game, &ray, x);
		x++;
	}
	return (0);
}
