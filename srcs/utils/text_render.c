/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   text_render.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kelle <kelle@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/30 04:00:00 by kelle             #+#    #+#             */
/*   Updated: 2026/06/23 05:00:38 by kelle            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

static const int	g_glyph_patterns[36][5] = {
{0b0111, 0b1001, 0b1001, 0b1001, 0b0111},
{0b0010, 0b0110, 0b0010, 0b0010, 0b0111},
{0b0111, 0b1001, 0b0010, 0b0100, 0b1111},
{0b1111, 0b0001, 0b0111, 0b0001, 0b1111},
{0b1000, 0b1100, 0b1010, 0b1111, 0b0010},
{0b1111, 0b1000, 0b1110, 0b0001, 0b1110},
{0b0110, 0b1000, 0b1110, 0b1001, 0b0110},
{0b1111, 0b0001, 0b0010, 0b0100, 0b1000},
{0b0110, 0b1001, 0b0110, 0b1001, 0b0110},
{0b0110, 0b1001, 0b0111, 0b0001, 0b0110},
{0b0110, 0b1001, 0b1111, 0b1001, 0b1001},
{0b1110, 0b1001, 0b1110, 0b1001, 0b1110},
{0b0111, 0b1000, 0b1000, 0b1000, 0b0111},
{0b1110, 0b1001, 0b1001, 0b1001, 0b1110},
{0b1111, 0b1000, 0b1110, 0b1000, 0b1111},
{0b1111, 0b1000, 0b1110, 0b1000, 0b1000},
{0b0111, 0b1000, 0b1011, 0b1001, 0b0111},
{0b1001, 0b1001, 0b1111, 0b1001, 0b1001},
{0b0111, 0b0010, 0b0010, 0b0010, 0b0111},
{0b1111, 0b0010, 0b0010, 0b1010, 0b0100},
{0b1001, 0b1010, 0b1100, 0b1010, 0b1001},
{0b1000, 0b1000, 0b1000, 0b1000, 0b1111},
{0b1001, 0b1111, 0b1111, 0b1001, 0b1001},
{0b1001, 0b1101, 0b1011, 0b1001, 0b1001},
{0b0110, 0b1001, 0b1001, 0b1001, 0b0110},
{0b1110, 0b1001, 0b1110, 0b1000, 0b1000},
{0b0110, 0b1001, 0b1001, 0b1010, 0b0101},
{0b1110, 0b1001, 0b1110, 0b1010, 0b1001},
{0b0111, 0b1000, 0b0110, 0b0001, 0b1110},
{0b1111, 0b0010, 0b0010, 0b0010, 0b0010},
{0b1001, 0b1001, 0b1001, 0b1001, 0b0111},
{0b1001, 0b1001, 0b1001, 0b0110, 0b0100},
{0b1001, 0b1001, 0b1111, 0b1111, 0b1001},
{0b1001, 0b0110, 0b0100, 0b0110, 0b1001},
{0b1001, 0b0110, 0b0100, 0b0100, 0b0100},
{0b1111, 0b0001, 0b0110, 0b1000, 0b1111}
};

static int	get_digit_pattern(char digit, int row)
{
	int	index;

	if ((digit >= '0' && digit <= '9') || (digit >= 'A' && digit <= 'Z')
		|| (digit >= 'a' && digit <= 'z'))
	{
		if (digit >= '0' && digit <= '9')
			index = digit - '0';
		else if (digit >= 'A' && digit <= 'Z')
			index = 10 + (digit - 'A');
		else
			index = 10 + (digit - 'a');
		if (index >= 0 && index < 36 && row >= 0 && row < 5)
			return (g_glyph_patterns[index][row]);
	}
	return (0);
}

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
		if ((str[i] >= '0' && str[i] <= '9') || (str[i] >= 'A' && str[i] <= 'Z')
			|| (str[i] >= 'a' && str[i] <= 'z'))
		{
			draw_char_scaled(game, str[i], char_pos, scale);
			offset += (4 + 1) * scale;
		}
		else if (str[i] == ':')
		{
			my_pixel_put(game, char_pos.x, pos.y + scale, 0xFFFFFF);
			my_pixel_put(game, char_pos.x, pos.y + 3 * scale, 0xFFFFFF);
			offset += 2 * scale;
		}
		else if (str[i] == '.')
		{
			my_pixel_put(game, char_pos.x, pos.y + 4 * scale, 0xFFFFFF);
			offset += 2 * scale;
		}
		else if (str[i] == ' ')
			offset += 2 * scale;
		i++;
	}
}
