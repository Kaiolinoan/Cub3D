/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   text_render.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kelle <kelle@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/30 04:00:00 by kelle             #+#    #+#             */
/*   Updated: 2026/06/02 19:11:46 by kelle            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

static int	get_digit_pattern(char digit, int row)
{
	static int patterns[36][5] = {
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

	if ((digit >= '0' && digit <= '9') || (digit >= 'A' && digit <= 'Z') 
		|| (digit >= 'a' && digit <= 'z'))
	{
		int index;
		if (digit >= '0' && digit <= '9')
			index = digit - '0';
		else if (digit >= 'A' && digit <= 'Z')
			index = 10 + (digit - 'A');
		else
			index = 10 + (digit - 'a');
		if (index >= 0 && index < 36 && row >= 0 && row < 5)
			return (patterns[index][row]);
	}
	return (0);
}

static void	draw_char_scaled(t_game *game, char c, int x, int y, int scale)
{
	int	row;
	int	col;
	int	pattern;
	int	px;
	int	py;
	int	sx;
	int	sy;
	int	offset;

	offset = 1;
	row = 0;
	while (row < 5)
	{
		pattern = get_digit_pattern(c, row);
		col = 0;
		while (col < 4)
		{
			if ((pattern >> (3 - col)) & 1)
			{
				sy = 0;
				while (sy < scale)
				{
					sx = 0;
					while (sx < scale)
					{
						px = x + col * scale + sx;
						py = y + row * scale + sy;
						if (px >= 0 && py >= 0 && px < game->win_w && py < game->win_h)
						my_pixel_put(game, px, py, 0x000000);
						sx++;
					}
					sy++;
				}
			}
			col++;
		}
		row++;
	}
	row = 0;
	while (row < 5)
	{
		pattern = get_digit_pattern(c, row);
		col = 0;
		while (col < 4)
		{
			if ((pattern >> (3 - col)) & 1)
			{
				sy = 0;
				while (sy < scale)
				{
					sx = 0;
					while (sx < scale)
					{
						px = x + offset + col * scale + sx;
						py = y + offset + row * scale + sy;
						if (px >= 0 && py >= 0 && px < game->win_w && py < game->win_h)
						my_pixel_put(game, px, py, 0xFFFFFF);
						sx++;
					}
					sy++;
				}
			}
			col++;
		}
		row++;
	}
}

void	draw_string_scaled(t_game *game, char *str, int x, int y, int scale)
{
	int	i;
	int	offset;

	i = 0;
	offset = 0;
	while (str[i])
	{
		if ((str[i] >= '0' && str[i] <= '9') || (str[i] >= 'A' && str[i] <= 'Z')
			|| (str[i] >= 'a' && str[i] <= 'z'))
		{
			draw_char_scaled(game, str[i], x + offset, y, scale);
			offset += (4 + 1) * scale;
		}
		else if (str[i] == ':')
		{
			my_pixel_put(game, x + offset, y + scale, 0xFFFFFF);
			my_pixel_put(game, x + offset, y + 3 * scale, 0xFFFFFF);
			offset += 2 * scale;
		}
		else if (str[i] == '.')
		{
			my_pixel_put(game, x + offset, y + 4 * scale, 0xFFFFFF);
			offset += 2 * scale;
		}
		else if (str[i] == ' ')
			offset += 2 * scale;
		i++;
	}
}

void	display_speeds(t_game *game)
{
	char	speed_str[50];
	char	rotation_str[50];
	char	full_speed[100];
	char	full_rotation[100];
	char	*int_str;
	char	*frac_str;
	int		speed_int;
	int		speed_frac;
	int		rot_int;
	int		rot_frac;

	speed_int = (int)game->movement_speed;
	speed_frac = (int)((game->movement_speed - speed_int) * 100);
	rot_int = (int)game->rotation_speed;
	rot_frac = (int)((game->rotation_speed - rot_int) * 1000);
	int_str = ft_itoa(speed_int);
	frac_str = ft_itoa(speed_frac);
	ft_strlcpy(speed_str, int_str, 50);
	ft_strlcat(speed_str, ".", 50);
	ft_strlcat(speed_str, frac_str, 50);
	free(int_str);
	free(frac_str);
	int_str = ft_itoa(rot_int);
	frac_str = ft_itoa(rot_frac);
	ft_strlcpy(rotation_str, int_str, 50);
	ft_strlcat(rotation_str, ".", 50);
	ft_strlcat(rotation_str, frac_str, 50);
	free(int_str);
	free(frac_str);
	ft_strlcpy(full_speed, "Movement Speed:", 100);
	ft_strlcat(full_speed, speed_str, 100);
	ft_strlcpy(full_rotation, "Rotation Speed:", 100);
	ft_strlcat(full_rotation, rotation_str, 100);
	draw_string_scaled(game, full_speed, game->win_w - 550, 20, 3);
	draw_string_scaled(game, full_rotation, game->win_w - 550, 50, 3);
}
