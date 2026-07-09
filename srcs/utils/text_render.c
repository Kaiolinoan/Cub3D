/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   text_render.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kelle <kelle@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/30 04:00:00 by kelle             #+#    #+#             */
/*   Updated: 2026/07/09 21:15:57 by kelle            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

static void	put_scaled_block(t_game *game, t_ip pos, int scale, int color)
{
	int	sx;
	int	sy;

	sy = 0;
	while (sy < scale)
	{
		sx = 0;
		while (sx < scale)
		{
			if (pos.x + sx >= 0 && pos.y + sy >= 0
				&& pos.x + sx < game->win_w && pos.y + sy < game->win_h)
				my_pixel_put(game, pos.x + sx, pos.y + sy, color);
			sx++;
		}
		sy++;
	}
}

static void	draw_glyph_layer(t_game *game, char c, t_ip pos,
	t_glyph_style style)
{
	int		row;
	int		col;
	int		pattern;
	t_ip	block_pos;

	row = 0;
	while (row < 5)
	{
		pattern = get_digit_pattern(c, row);
		col = 0;
		while (col < 4)
		{
			if ((pattern >> (3 - col)) & 1)
			{
				block_pos.x = pos.x + col * style.scale;
				block_pos.y = pos.y + row * style.scale;
				put_scaled_block(game, block_pos, style.scale, style.color);
			}
			col++;
		}
		row++;
	}
}

static void	draw_char_scaled(t_game *game, char c, t_ip pos, int scale)
{
	t_glyph_style	style;
	t_ip			offset_pos;

	style.scale = scale;
	style.color = 0x000000;
	draw_glyph_layer(game, c, pos, style);
	offset_pos.x = pos.x + 1;
	offset_pos.y = pos.y + 1;
	style.color = 0xFFFFFF;
	draw_glyph_layer(game, c, offset_pos, style);
}

static int	draw_punct_char(t_game *game, char c, t_ip pos, int scale)
{
	if (c == ':')
	{
		my_pixel_put(game, pos.x, pos.y + scale, 0xFFFFFF);
		my_pixel_put(game, pos.x, pos.y + 3 * scale, 0xFFFFFF);
		return (2 * scale);
	}
	if (c == '.')
	{
		my_pixel_put(game, pos.x, pos.y + 4 * scale, 0xFFFFFF);
		return (2 * scale);
	}
	if (c == ' ')
		return (2 * scale);
	return (0);
}

void	draw_string_scaled(t_game *game, char *str, t_ip pos, int scale)
{
	int		i;
	int		offset;
	t_ip	char_pos;

	i = 0;
	offset = 0;
	while (str[i])
	{
		char_pos.x = pos.x + offset;
		char_pos.y = pos.y;
		if ((str[i] >= '0' && str[i] <= '9') || (str[i] >= 'A'
				&& str[i] <= 'Z') || (str[i] >= 'a' && str[i] <= 'z'))
		{
			draw_char_scaled(game, str[i], char_pos, scale);
			offset += (4 + 1) * scale;
		}
		else
			offset += draw_punct_char(game, str[i], char_pos, scale);
		i++;
	}
}
