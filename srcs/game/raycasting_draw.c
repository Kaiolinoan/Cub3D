/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   raycasting_draw.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kelle <kelle@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/07 20:04:17 by kelle             #+#    #+#             */
/*   Updated: 2026/07/07 20:05:30 by kelle            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

void	draw_textured_line(t_game *game, t_ray *ray, int x, int y)
{
	double			step;
	double			tex_pos;
	int				tex_y;
	unsigned int	color;

	step = (double)PX / ray->line_height;
	tex_pos = (ray->draw_start - game->win_h / 2
			+ ray->line_height / 2) * step;
	y = ray->draw_start;
	(load_texture(game, ray), find_wall_hit(game, ray));
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
		if (!ray->is_door || color)
			my_pixel_put(game, x, y, color);
		y++;
	}
}
