#include "cub3d.h"

int	finish_game(void *param)
{
	t_game	*game;

	game = param;
	clear_game(game);
	exit(EXIT_SUCCESS);
	return (0);
}

// int mouse_move(int x, int y, void *param)
// {
// 	t_game	*game;
// 	int delta_x;
// 	int center_x;
// 	int center_y;

// 	game = param;
// 	center_x = game->win_w / 2;
// 	center_y = game->win_h / 2;
// 	if (x == center_x && y == center_y)
// 		return (0);
// 	delta_x = x - center_x;
// 	rotate(&game->player, delta_x  * game->rotation_speed * 0.01);
// 	// mlx_mouse_move(game->mlx, game->win, center_x, center_y);
// 	// mlx_mouse_hide(game->mlx, game->win);
// 	// printf("mouse\n");
	// game->player.mouse_x = delta_x;
// 	return (0);
// }

static int	mouse_move(int x, int y, void *param)
{
	t_game		*game;
	static int	last_x = -1;
	int			dx;

	(void)y;
	game = param;
	if (game->player.mouse_flag == false)
		return (0);
	if (last_x == -1)
	{
		last_x = x;
		return (0);
	}
	dx = x - last_x;
	last_x = x;
	rotate(&game->player, dx * game->rotation_speed * 0.1);
	return (0);
}

void	mlx_main(t_game *game)
{
	game->mlx = mlx_init();
	if (!game->mlx)
		return (print_error(MLX), clear_game(game));
	if (game->debug)
	{
		game->win_w = ft_strlen(*game->map->grid) * 64;
		game->win_h = array_len(game->map->grid) * 64;
	}
	else
		mlx_get_screen_size(game->mlx, &game->win_w, &game->win_h);
	game->win = mlx_new_window(game->mlx, game->win_w, game->win_h, "Cub3D");
	if (!game->win)
		return (print_error(MLX_WIN), clear_game(game));
	if (!initialize_images(game, &game->sprites))
		return ;
	initialize_player(game, &game->player);
	mlx_hook(game->win, KeyPress, KeyPressMask, player_moving, game);
	mlx_hook(game->win, KeyRelease, KeyReleaseMask, player_idle, game);
	mlx_hook(game->win, 6, PointerMotionMask, mouse_move, game);
	mlx_hook(game->win, DestroyNotify, StructureNotifyMask, finish_game, game);
	mlx_loop_hook(game->mlx, &render, game);
	mlx_loop(game->mlx);
}
