/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mlx.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klino-an <klino-an@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/10 19:12:00 by kelle             #+#    #+#             */
/*   Updated: 2026/07/07 20:49:40 by klino-an         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

int	finish_game(void *param)
{
	t_game	*game;

	game = param;
	clear_game(game);
	exit(EXIT_SUCCESS);
	return (0);
}

int	mouse_move(int x, int y, void *param)
{
	t_game	*game;
	int		delta_x;
	int		center_x;
	int		center_y;

	game = param;
	center_x = game->win_w / 2;
	center_y = game->win_h / 2;
	if ((x == center_x && y == center_y) || (game->player.mouse_flag == false))
		return (0);
	delta_x = x - center_x;
	rotate(&game->player, delta_x * game->rotation_speed * 0.01);
	mlx_mouse_move(game->mlx, game->win, center_x, center_y);
	mlx_mouse_hide(game->mlx, game->win);
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
	init_door_audio();
	mlx_hook(game->win, KeyPress, KeyPressMask, player_moving, game);
	mlx_hook(game->win, KeyRelease, KeyReleaseMask, player_idle, game);
	mlx_hook(game->win, 6, PointerMotionMask, mouse_move, game);
	mlx_hook(game->win, DestroyNotify, StructureNotifyMask, finish_game, game);
	mlx_loop_hook(game->mlx, &render, game);
	mlx_loop(game->mlx);
}
