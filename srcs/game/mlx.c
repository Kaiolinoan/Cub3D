#include "cub3d.h"

static int	key_inputs(int keycode, void *param)
{
	t_game	*game;

	game = param;
	if (keycode == XK_Escape)
	{
		clear_game(game);
		exit(EXIT_SUCCESS);
	}
	return (0);
}

int	finish_game(void *param)
{
	t_game	*game;

	game = param;
	clear_game(game);
	exit(EXIT_SUCCESS);
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
	game->win = mlx_new_window(game->mlx, game->win_w / 1.1, game->win_h / 1.1, "Cub3D");
	if (!game->win)
		return (print_error(MLX_WIN), clear_game(game));
	if (!initialize_images(game, &game->sprites))
		return ;
	initialize_player(game, &game->player);
	mlx_key_hook(game->win, key_inputs, game);
	mlx_hook(game->win, KeyPress, KeyPressMask, player_moving, game);
	mlx_hook(game->win, KeyRelease, KeyReleaseMask, player_idle, game);
	mlx_hook(game->win, DestroyNotify, StructureNotifyMask, finish_game, game);
	mlx_loop_hook(game->mlx, &render, game);
	mlx_loop(game->mlx);
}
