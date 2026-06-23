/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   text_render_utils.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kelle <kelle@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/22 00:00:00 by kelle             #+#    #+#             */
/*   Updated: 2026/06/23 05:01:02 by kelle            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

static char	*format_speed(double value, int multiplier)
{
	char	*int_str;
	char	*frac_str;
	char	*tmp;

	int_str = ft_itoa((int)value);
	frac_str = ft_itoa((int)((value - (int)value) * multiplier));
	tmp = ft_strjoin_and_free(int_str, ".");
	tmp = ft_strjoin_and_free(tmp, frac_str);
	free(frac_str);
	return (tmp);
}

void	display_speeds(t_game *game)
{
	char	*speed_str;
	char	*rotation_str;
	char	*full_speed;
	char	*full_rotation;
	t_ip	pos;

	speed_str = format_speed(game->movement_speed, 100);
	rotation_str = format_speed(game->rotation_speed, 1000);
	full_speed = ft_strjoin_and_free(ft_strdup("Movement Speed:"), speed_str);
	full_rotation = ft_strjoin_and_free(ft_strdup("Rotation Speed:"),
			rotation_str);
	free(speed_str);
	free(rotation_str);
	pos.x = game->win_w - 550;
	pos.y = 20;
	draw_string_scaled(game, full_speed, pos, 3);
	pos.y = 50;
	draw_string_scaled(game, full_rotation, pos, 3);
	free(full_speed);
	free(full_rotation);
}
