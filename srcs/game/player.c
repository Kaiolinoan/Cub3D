/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   player.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kelle <kelle@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/23 02:26:21 by kelle             #+#    #+#             */
/*   Updated: 2026/06/01 05:52:53 by kelle            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

static void	handle_keys(int keycode, t_player *player)
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

// static void	handle_minimap_keys(int keycode, t_game *game)
// {
// 	if (keycode == XK_m)
// 		display_minimap(game);
// 	else if (keycode == XK_plus || keycode == XK_equal)
// 		game->minimap_radius_tiles += 0.5;
// 	else if (keycode == XK_minus)
// 	{
// 		if (game->minimap_radius_tiles > 1.0)
// 			game->minimap_radius_tiles -= 0.5;
// 	}
// 	else if (keycode == XK_bracketright)
// 		game->minimap_tile_size += 2;
// 	else if (keycode == XK_bracketleft)
// 	{
// 		if (game->minimap_tile_size > 4)
// 			game->minimap_tile_size -= 2;
// 	}
// }

static void	handle_speed_keys(int keycode, t_game *game)
{
	if (keycode == XK_1)
	{
		if (game->movement_speed > 0.01)
			game->movement_speed -= 0.01;
	}
	else if (keycode == XK_2)
		game->movement_speed += 0.01;
	else if (keycode == XK_comma)
	{
		if (game->rotation_speed > 0.005)
			game->rotation_speed -= 0.005;
	}
	else if (keycode == XK_period)
		game->rotation_speed += 0.005;
}

int	player_moving(int keycode, void *param)
{
	t_game	*game;

	game = param;
	if (keycode == XK_Escape)
		finish_game(game);
	handle_keys(keycode, &game->player);
	// handle_minimap_keys(keycode, game);
	handle_speed_keys(keycode, game);
	return (0);
}

int	player_idle(int keycode, void *param)
{
	t_game	*game;

	game = param;
	if (keycode == XK_w)
		game->player.up = false;
	else if (keycode == XK_s)
		game->player.down = false;
	else if (keycode == XK_a)
		game->player.left = false;
	else if (keycode == XK_d)
		game->player.right = false;
	else if (keycode == XK_Left)
		game->player.rotate_l = false;
	else if (keycode == XK_Right)
		game->player.rotate_r = false;
	return (0);
}

static void	set_player_dir_values(t_player *player, double dx, double dy)
{
	player->dir_x = dx;
	player->dir_y = dy;
}

static void	set_player_plane_values(t_player *player, double px, double py)
{
	player->plane_x = px;
	player->plane_y = py;
}

void	initialize_player(t_game *game, t_player *player)
{
	player->player_x = game->map->start_x;
	player->player_y = game->map->start_y;
	player->mouse_flag = false;
	if (player->starting_direction == NORTH)
	{
		set_player_dir_values(player, 0, -1);
		set_player_plane_values(player, game->fov, 0);
	}
	else if (player->starting_direction == SOUTH)
	{
		set_player_dir_values(player, 0, 1);
		set_player_plane_values(player, -game->fov, 0);
	}
	else if (player->starting_direction == EAST)
	{
		set_player_dir_values(player, 1, 0);
		set_player_plane_values(player, 0, game->fov);
	}
	else if (player->starting_direction == WEST)
	{
		set_player_dir_values(player, -1, 0);
		set_player_plane_values(player, 0, -game->fov);
	}
}
