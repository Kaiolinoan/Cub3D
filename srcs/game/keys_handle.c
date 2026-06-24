/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   keys_handle.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kelle <kelle@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/10 19:11:55 by kelle             #+#    #+#             */
/*   Updated: 2026/06/23 04:40:05 by kelle            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

void	handle_keys(int keycode, t_player *player)
{
	if (keycode == XK_w)
		player->up = true;
	else if (keycode == XK_s)
		player->down = true;
	else if (keycode == XK_a)
		player->left = true;
	else if (keycode == XK_d)
		player->right = true;
	else if (keycode == XK_Left)
		player->rotate_l = true;
	else if (keycode == XK_Right)
		player->rotate_r = true;
	if (keycode == XK_space)
	{
		if (player->mouse_flag == false)
			player->mouse_flag = true;
		else
			player->mouse_flag = false;
	}
}

static void	handle_minimap_size(int keycode, t_game *game)
{
	if (keycode == XK_plus || keycode == XK_equal)
		game->minimap_radius_tiles += 0.5;
	else if (keycode == XK_minus)
	{
		if (game->minimap_radius_tiles > 1.0)
			game->minimap_radius_tiles -= 0.5;
	}
}

static void	handle_minimap_zoom(int keycode, t_game *game)
{
	double	radius_px;

	if (keycode == XK_bracketright)
	{
		if (game->minimap_tile_size < 40)
		{
			radius_px = game->minimap_radius_tiles * game->minimap_tile_size;
			game->minimap_tile_size += 2;
			game->minimap_radius_tiles = radius_px / game->minimap_tile_size;
		}
	}
	else if (keycode == XK_bracketleft)
	{
		if (game->minimap_tile_size > 4)
		{
			radius_px = game->minimap_radius_tiles * game->minimap_tile_size;
			game->minimap_tile_size -= 2;
			game->minimap_radius_tiles = radius_px / game->minimap_tile_size;
		}
	}
}

void	handle_minimap_keys(int keycode, t_game *game)
{
	if (keycode == XK_m)
		display_minimap(game);
	handle_minimap_size(keycode, game);
	handle_minimap_zoom(keycode, game);
}

void	handle_speed_keys(int keycode, t_game *game)
{
	if (keycode == XK_1)
	{
		if (game->movement_speed > 0.01)
			game->movement_speed -= 0.01;
	}
	else if (keycode == XK_2)
	{
		if (game->movement_speed < 0.7)
			game->movement_speed += 0.01;
	}
	else if (keycode == XK_Down)
	{
		if (game->rotation_speed > 0.005)
			game->rotation_speed -= 0.005;
	}
	else if (keycode == XK_Up)
		game->rotation_speed += 0.005;
}
