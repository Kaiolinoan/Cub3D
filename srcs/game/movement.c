#include "cub3d.h"

static void	rotate(t_player *player, double rot)
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

static void update_player_x_coordinate(t_game *game, double direction_x, double speed, bool sum)
{
	double new_x;
	double padding;
	double collision_x;

	padding = 0.2;
	if (sum)
	{
		new_x = game->player.player_x + direction_x * speed;
		collision_x = new_x + direction_x * padding;
	}
	else
	{
		new_x = game->player.player_x - direction_x * speed;
		collision_x = new_x - direction_x * padding;
	}
	if (game->map->grid[(int)game->player.player_y][(int)collision_x] == '1')
		return ;
	game->player.player_x = new_x;
}

static void update_player_y_coordinate(t_game *game, double direction_y, double speed, bool sum)
{
	double new_y;
	double padding;
	double collision_y;

	padding = 0.2;
	if (sum)
	{
		new_y = game->player.player_y + direction_y * speed;
		collision_y = new_y + direction_y * padding;
	}
	else
	{
		new_y = game->player.player_y - direction_y * speed;
		collision_y = new_y - direction_y * padding;
	}
	if (game->map->grid[(int)collision_y][(int)game->player.player_x] == '1')
		return ;
	game->player.player_y = new_y;
}

static void check_direction_to_move(t_game *game, t_player *player)
{
	double	speed;

	speed = 0.07;
	if (player->up)
	{
		update_player_x_coordinate(game, player->dir_x, speed, true);
		update_player_y_coordinate(game, player->dir_y, speed, true);
	}
	if (player->down)
	{
		update_player_x_coordinate(game, player->dir_x, speed, false);
		update_player_y_coordinate(game, player->dir_y, speed, false);
	}
	if (player->left)
    {
		update_player_x_coordinate(game, player->plane_x, speed, false);
		update_player_y_coordinate(game, player->plane_y, speed, false);
    }
	if (player->right)
    {
		update_player_x_coordinate(game, player->plane_x, speed, true);
		update_player_y_coordinate(game, player->plane_y, speed, true);
    }
}

void	move_player(t_game *game, t_player *player)
{
	double	rot_speed;

	rot_speed = 0.03;
	check_direction_to_move(game, player);
	if (player->rotate_l)
		rotate(player, -rot_speed);
	if (player->rotate_r)
		rotate(player, rot_speed);
}
