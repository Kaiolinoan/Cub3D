/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   player.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kelle <kelle@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/23 02:26:21 by kelle             #+#    #+#             */
/*   Updated: 2026/05/23 02:32:31 by kelle            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

int	player_moving(int keycode, void *param)
{
	t_game	*game;

	game = param;
	if (keycode == XK_w)
		game->player.up = true;
	else if (keycode == XK_s)
		game->player.down = true;
	else if (keycode == XK_a)
		game->player.left = true;
	else if (keycode == XK_d)
		game->player.right = true;
	else if (keycode == XK_Left)
		game->player.rotate_l = true;
	else if (keycode == XK_Right)
		game->player.rotate_r = true;
	else if (keycode == XK_Escape)
		finish_game(game);
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

void	rotate(t_player *player, double rot)
{
	double	old_dir_x;
	double	old_dir_y;
	double	old_plane_x;
	double	old_plane_y;

	old_dir_x = player->dir_x;
	old_dir_y = player->dir_y;
	old_plane_x = player->plane_x;
	old_plane_y = player->plane_y;

	player->dir_x = old_dir_x * cos(rot) - old_dir_y * sin(rot);
	player->dir_y = old_dir_x * sin(rot) + old_dir_y * cos(rot);
	player->plane_x = old_plane_x * cos(rot) - old_plane_y * sin(rot);
	player->plane_y = old_plane_x * sin(rot) + old_plane_y * cos(rot);
}

void	move_player(t_player *player)
{
	double	speed;
	double	rot_speed;

	speed = 0.1;
	rot_speed = 0.03;
	if (player->up)
	{
		player->player_x += player->dir_x * speed;
		player->player_y += player->dir_y * speed;
	}
	if (player->down)
	{
		player->player_x -= player->dir_x * speed;
		player->player_y -= player->dir_y * speed;
	}
	if (player->left)
    {
	    player->player_x -= player->plane_x * speed;
	    player->player_y -= player->plane_y * speed;
    }
	if (player->right)
    {
	    player->player_x += player->plane_x * speed;
	    player->player_y += player->plane_y * speed;
    }
	if (player->rotate_l)
		rotate(player, -rot_speed);
	if (player->rotate_r)
		rotate(player, rot_speed);
}

void	initialize_player(t_game *game, t_player *player)
{
	player->player_x = game->map->start_x;
	player->player_y = game->map->start_y;
	if (player->starting_direction == NORTH)
	{
		player->dir_x = 0;
		player->dir_y = -1;
        player->plane_x = 0.66;
        player->plane_y = 0;
	}
	if (player->starting_direction == SOUTH)
	{
		player->dir_x = 0;
		player->dir_y = 1;
        player->plane_x = -0.66;
        player->plane_y = 0;
	}
	if (player->starting_direction == EAST)
	{
		player->dir_x = 1;
		player->dir_y = 0;
        player->plane_x = 0;
        player->plane_y = 0.66;
	}
	if (player->starting_direction == WEST)
	{
		player->dir_x = -1;
		player->dir_y = 0;
        player->plane_x = 0;
        player->plane_y = -0.66;
	}
}

