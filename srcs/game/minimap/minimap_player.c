/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minimap_player.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kelle <kelle@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/10 19:26:49 by kelle             #+#    #+#             */
/*   Updated: 2026/06/16 01:29:34 by kelle            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

void	draw_minimap_player(t_game *game, int center_x, int center_y)
{
	int		marker_offset_x;
	int		marker_offset_y;
	double	radius;
	double	radius_sq;

	radius = game->minimap_tile_size * 0.3;
	radius_sq = radius * radius;
	marker_offset_y = -(int)radius;
	while (marker_offset_y <= (int)radius)
	{
		marker_offset_x = -(int)radius;
		while (marker_offset_x <= (int)radius)
		{
			if ((marker_offset_x * marker_offset_x
					+ marker_offset_y * marker_offset_y) <= radius_sq)
				put_minimap_pixel(game, center_x + marker_offset_x,
					center_y + marker_offset_y, MINIMAP_PLAYER_COLOR);
			marker_offset_x++;
		}
		marker_offset_y++;
	}
}
