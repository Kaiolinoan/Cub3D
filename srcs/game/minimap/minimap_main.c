/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minimap_main.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kelle <kelle@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/27 03:26:05 by kelle             #+#    #+#             */
/*   Updated: 2026/06/23 03:45:55 by kelle            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

static void	render_background_pixel(t_game *game, t_ip p, int radius_px,
	t_ip offset)
{
	double	poffx;
	double	poffy;
	double	dist_sq;

	poffx = p.x - radius_px;
	poffy = p.y - radius_px;
	dist_sq = poffx * poffx + poffy * poffy;
	if (dist_sq <= (double)(radius_px * radius_px))
		put_minimap_pixel(game, offset.x + p.x, offset.y + p.y,
			minimap_tile_color(game, game->player.player_x
				+ (poffx / (double)game->minimap_tile_size),
				game->player.player_y
				+ (poffy / (double)game->minimap_tile_size)));
}

static void	render_minimap_background(t_game *game, int radius_px,
		int diameter, t_ip offset)
{
	int	pixel_y;
	int	pixel_x;

	pixel_y = 0;
	while (pixel_y < diameter)
	{
		pixel_x = 0;
		while (pixel_x < diameter)
		{
			render_background_pixel(game, (t_ip){pixel_x, pixel_y},
				radius_px, offset);
			pixel_x++;
		}
		pixel_y++;
	}
}

static void	render_minimap_border(t_game *game, int radius_px,
		int diameter, t_ip offset)
{
	int		pixel_y;
	int		pixel_x;
	double	pixel_offset_x;
	double	pixel_offset_y;
	double	dist_sq;

	pixel_y = 0;
	while (pixel_y < diameter)
	{
		pixel_x = 0;
		while (pixel_x < diameter)
		{
			pixel_offset_x = pixel_x - radius_px;
			pixel_offset_y = pixel_y - radius_px;
			dist_sq = pixel_offset_x * pixel_offset_x
				+ pixel_offset_y * pixel_offset_y;
			if (dist_sq <= (double)(radius_px * radius_px)
					&& dist_sq >= (double)((radius_px - 2) * (radius_px - 2)))
				put_minimap_pixel(game, offset.x + pixel_x, offset.y + pixel_y,
					0x00555555);
			pixel_x++;
		}
		pixel_y++;
	}
}

void	render_minimap(t_game *game)
{
	int	radius_px;
	int	diameter;
	int	offset_x;
	int	offset_y;

	radius_px = (int)(game->minimap_radius_tiles * game->minimap_tile_size);
	diameter = radius_px * 2;
	offset_x = MINIMAP_MARGIN;
	offset_y = MINIMAP_MARGIN;
	render_minimap_background(game, radius_px, diameter,
		(t_ip){offset_x, offset_y});
	draw_minimap_fov(game, offset_x + radius_px, offset_y + radius_px);
	render_minimap_border(game, radius_px, diameter,
		(t_ip){offset_x, offset_y});
	draw_minimap_player(game, offset_x + radius_px, offset_y + radius_px);
}
