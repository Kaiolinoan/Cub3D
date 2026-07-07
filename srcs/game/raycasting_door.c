/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   raycasting_door.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kelle <kelle@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/07 20:06:58 by kelle             #+#    #+#             */
/*   Updated: 2026/07/07 20:07:05 by kelle            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

static int	door_tex_x(t_ray *ray, double wall_x)
{
	int	tex_x;

	tex_x = (int)(wall_x * PX);
	if (ray->side == 0 && ray->ray_dir_x > 0)
		tex_x = PX - tex_x - 1;
	if (ray->side == 1 && ray->ray_dir_y < 0)
		tex_x = PX - tex_x - 1;
	return (tex_x);
}

bool	ray_hits_door(t_game *game, t_ray *ray)
{
	t_door	*door;
	double	perp;
	double	wall_x;
	int		tex_x;
	int		open_px;

	door = get_door_at(game, ray->map_x, ray->map_y);
	if (!door || door->state == DOOR_CLOSED)
		return (true);
	if (door->state == DOOR_OPEN || door->frame >= DOOR_ANIM_FRAMES - 1)
		return (false);
	if (ray->side == 0)
		perp = ray->side_dist_x - ray->delta_dist_x;
	else
		perp = ray->side_dist_y - ray->delta_dist_y;
	if (ray->side == 0)
		wall_x = game->player.player_y + perp * ray->ray_dir_y;
	else
		wall_x = game->player.player_x + perp * ray->ray_dir_x;
	wall_x -= floor(wall_x);
	tex_x = door_tex_x(ray, wall_x);
	open_px = (int)((double)door->frame / DOOR_ANIM_FRAMES * (PX / 2));
	return (abs(tex_x - PX / 2) >= open_px);
}
