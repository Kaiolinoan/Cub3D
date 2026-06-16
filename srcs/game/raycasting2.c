/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   raycasting2.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kelle <kelle@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/10 19:12:13 by kelle             #+#    #+#             */
/*   Updated: 2026/06/10 19:12:14 by kelle            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

static void	load_texture(t_game *game, t_ray *ray)
{
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

static void	find_wall_hit(t_game *game, t_ray *ray)
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

static unsigned int	get_texture_pixel(t_img *texture, int x, int y)
{
	int		offset;

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

void	draw_textured_line(t_game *game, t_ray *ray, int x)
{
	double			step;
	double			tex_pos;
	int				tex_y;
	int				y;
	unsigned int	color;

	step = (double)PX / ray->line_height;
	tex_pos = (ray->draw_start - game->win_h / 2 + ray->line_height / 2) * step;
	y = ray->draw_start;
	load_texture(game, ray);
	find_wall_hit(game, ray);
	while (y < ray->draw_end)
	{
		tex_y = (int)tex_pos & (PX - 1);
		tex_pos += step;
		if (ray->tex_x < 0 || ray->tex_x >= PX || tex_y < 0 || tex_y >= PX)
		{
			y++;
			continue ;
		}
		color = color_gradient(ray,
				get_texture_pixel(&ray->texture.img, ray->tex_x, tex_y));
		my_pixel_put(game, x, y, color);
		y++;
	}
}
