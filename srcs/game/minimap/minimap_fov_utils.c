/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minimap_fov_utils.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kelle <kelle@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/10 19:47:23 by kelle             #+#    #+#             */
/*   Updated: 2026/06/16 02:11:07 by kelle            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

unsigned int	blend_color_with_white(unsigned int base_color,
		double white_opacity)
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

bool	minimap_in_circle(int x, int y, t_ip center, int radius_px)
{
	int	center_offset_x_sq;
	int	center_offset_y_sq;

	center_offset_x_sq = (x - center.x) * (x - center.x);
	center_offset_y_sq = (y - center.y) * (y - center.y);
	return (center_offset_x_sq + center_offset_y_sq <= radius_px * radius_px);
}

static void	fov_line_step(double *x, double *y, double sx, double sy)
{
	if (ft_abs(sx) > ft_abs(sy))
	{
		*x += sx / ft_abs(sx);
		*y += sy / ft_abs(sx);
	}
	else
	{
		*x += sx / ft_abs(sy);
		*y += sy / ft_abs(sy);
	}
}

static void	fov_line_pixel(t_game *game, t_ip start, double lx, double ly)
{
	int	px;
	int	py;

	px = (int)(lx + 0.5);
	py = (int)(ly + 0.5);
	if (minimap_in_circle(px, py, start,
			(int)(game->minimap_radius_tiles * game->minimap_tile_size)))
		put_minimap_pixel(game, px, py,
			blend_color_with_white(rgb_to_color(game->map->floor), 0.2));
}

void	draw_minimap_fov_line(t_game *game, t_ip start, t_dp end)
{
	double	sx;
	double	sy;
	double	lx;
	double	ly;
	int		steps;

	sx = end.x - start.x;
	sy = end.y - start.y;
	steps = ft_abs(sx);
	if (ft_abs(sy) > steps)
		steps = ft_abs(sy);
	if (steps <= 0)
		steps = 1;
	lx = start.x;
	ly = start.y;
	while (steps-- >= 0)
	{
		fov_line_pixel(game, start, lx, ly);
		fov_line_step(&lx, &ly, sx, sy);
	}
}
