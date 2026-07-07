/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   raycasting2.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kelle <kelle@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/07 20:03:53 by kelle             #+#    #+#             */
/*   Updated: 2026/07/07 20:19:40 by kelle            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

static void	load_door_texture(t_game *game, t_ray *ray)
{
	t_door	*door;
	int		frame;

	door = get_door_at(game, ray->map_x, ray->map_y);
	if (!door)
		return ;
	frame = door->frame;
	if (frame >= DOOR_ANIM_FRAMES)
		frame = DOOR_ANIM_FRAMES - 1;
	if (frame < 0)
		frame = 0;
	ray->texture = game->sprites.door_frames[frame];
}

void	load_texture(t_game *game, t_ray *ray)
{
	if (ray->is_door)
		return (load_door_texture(game, ray));
	if (ray->side == 0)
	{
		if (ray->ray_dir_x > 0)
			ray->texture = game->sprites.east;
		else
			ray->texture = game->sprites.west;
	}
	else
	{
		if (ray->ray_dir_y > 0)
			ray->texture = game->sprites.south;
		else
			ray->texture = game->sprites.north;
	}
}

void	find_wall_hit(t_game *game, t_ray *ray)
{
	double	wall_x;

	if (ray->side == 0)
		wall_x = game->player.player_y + ray->perp_wall_dist * ray->ray_dir_y;
	else
		wall_x = game->player.player_x + ray->perp_wall_dist * ray->ray_dir_x;
	wall_x -= floor(wall_x);
	ray->tex_x = (int)(wall_x * PX);
	if (ray->side == 0 && ray->ray_dir_x > 0)
		ray->tex_x = PX - ray->tex_x - 1;
	if (ray->side == 1 && ray->ray_dir_y < 0)
		ray->tex_x = PX - ray->tex_x - 1;
}

unsigned int	get_texture_pixel(t_img *texture, int x, int y)
{
	int	offset;

	offset = y * texture->line_length + x * (texture->bits_per_pixel / 8);
	return (*(unsigned int *)(texture->addr + offset));
}

void	calculate_wall(t_game *game, t_ray *ray)
{
	if (ray->side == 0)
		ray->perp_wall_dist = ray->side_dist_x - ray->delta_dist_x;
	else
		ray->perp_wall_dist = ray->side_dist_y - ray->delta_dist_y;
	if (ray->perp_wall_dist == 0)
		ray->perp_wall_dist = 0.0001;
	ray->line_height = (int)(game->win_h / ray->perp_wall_dist);
	ray->draw_start = -ray->line_height / 2 + game->win_h / 2;
	if (ray->draw_start < 0)
		ray->draw_start = 0;
	ray->draw_end = ray->line_height / 2 + game->win_h / 2;
	if (ray->draw_end >= game->win_h)
		ray->draw_end = game->win_h - 1;
}
