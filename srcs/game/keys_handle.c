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

void	handle_minimap_keys(int keycode, t_game *game)
{
	if (keycode == XK_m)
		display_minimap(game);
	else if (keycode == XK_plus || keycode == XK_equal)
		game->minimap_radius_tiles += 0.5;
	else if (keycode == XK_minus)
	{
		if (game->minimap_radius_tiles > 1.0)
			game->minimap_radius_tiles -= 0.5;
	}
	else if (keycode == XK_bracketright)
		game->minimap_tile_size += 2;
	else if (keycode == XK_bracketleft)
	{
		if (game->minimap_tile_size > 4)
			game->minimap_tile_size -= 2;
	}
}

void	handle_speed_keys(int keycode, t_game *game)
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
