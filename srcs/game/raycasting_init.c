/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   raycasting_init.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kelle <kelle@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/07 20:03:39 by kelle             #+#    #+#             */
/*   Updated: 2026/07/07 20:11:38 by kelle            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

static void	init_ray(t_game *game, t_player *player, t_ray *ray, int x)
{
	ray->camera_x = 2 * x / (double)game->win_w - 1;
	ray->ray_dir_x = player->dir_x + player->plane_x * ray->camera_x;
	ray->ray_dir_y = player->dir_y + player->plane_y * ray->camera_x;
	ray->map_x = (int)player->player_x;
	ray->map_y = (int)player->player_y;
	ray->hit = false;
	ray->is_door = false;
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
		cast_ray(game, &ray, 0);
		calculate_wall(game, &ray);
		draw_textured_line(game, &ray, x, 0);
		x++;
	}
	return (0);
}
