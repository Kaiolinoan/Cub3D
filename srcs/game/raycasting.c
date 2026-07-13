/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   raycasting.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kelle <kelle@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/07 20:03:15 by kelle             #+#    #+#             */
/*   Updated: 2026/07/07 20:12:09 by kelle            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

bool	out_of_bounds(t_game *game, t_ray *ray)
{
	if (ray->map_y < 0 || ray->map_y >= game->win_h || ray->map_x < 0)
		return (true);
	if (ray->map_x >= (int)ft_strlen(game->map->grid[ray->map_y]))
		return (true);
	return (false);
}

void	cast_ray(t_game *game, t_ray *ray, char tile)
{
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
		if (out_of_bounds(game, ray))
			return ;
		tile = game->map->grid[ray->map_y][ray->map_x];
		if (tile == '1')
			ray->hit = 1;
		else if (tile == 'D' && ray_hits_door(game, ray))
		{
			ray->hit = 1;
			ray->is_door = true;
		}
	}
}

void	check_raydir_x(t_player *player, t_ray *ray)
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

void	check_raydir_y(t_player *player, t_ray *ray)
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
