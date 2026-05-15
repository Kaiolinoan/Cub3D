/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kelle <kelle@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/16 01:10:31 by kelle             #+#    #+#             */
/*   Updated: 2026/05/15 19:14:48 by kelle            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

static int	key_inputs(int keycode, void *param)
{
	t_game *game = param;
	if (keycode == XK_Escape)
	{
		clear_game(game);
		exit(EXIT_SUCCESS);
	}
	return (0);
}

static int	finish_game(void *param)
{
	t_game *game = param;
	clear_game(game);
	exit(EXIT_SUCCESS);
	return (0);
}

static void	mlx_main(t_game *game)
{
	int	screen_w;
	int	screen_h;
	int	win_w;
	int	win_h;

	game->mlx = mlx_init();
	if (!game->mlx)
		return (print_error(MLX), clear_game(game));
	mlx_get_screen_size(game->mlx, &screen_w, &screen_h);
	win_w = screen_w / 1.5;
	win_h = screen_h / 1.5;
	game->win = mlx_new_window(game->mlx, win_w, win_h, "Cub3D");
	if (!game->win)
		return (print_error(MLX_WIN), clear_game(game));
	mlx_key_hook(game->win, key_inputs, game);
	mlx_hook(game->win, DestroyNotify, StructureNotifyMask, finish_game, game);
	mlx_loop(game->mlx);
}

static t_game	*initialize_game(char *filename)
{
	t_game	*game;

	game = ft_calloc(1, sizeof(t_game));
	if (!game)
		return (print_error(ALLOC_ERR), NULL);
	game->map = ft_calloc(1, sizeof(t_map));
	if (!game->map)
		return (clear_game(game), print_error(ALLOC_ERR), NULL);
	if (!get_map_details(game, filename))
		return (clear_game(game), NULL);
	return (game);
}

int	main(int argc, char **argv)
{
	t_game	*game;

	if (argc != 2)
		return (print_error("Invalid argument"), 1);
	game = initialize_game(argv[1]);
	if (!game)
		return (1);
	if (!check_if_map_is_valid(argv[1], game))
		return (clear_game(game), 1);
	printf("passou pelo parsing\n");
	mlx_main(game);
	clear_game(game);
	return (0);
}

	// printf("EA: %s\n", (char *)game->img.east);
	// printf("WE: %s\n", (char *)game->img.west);
	// printf("SO: %s\n", (char *)game->img.south);
	// printf("NO: %s\n", (char *)game->img.north);
	// printf("C: %d, %d, %d\n", game->map->ceiling->r, game->map->ceiling->g, game->map->ceiling->b);
	// printf("F: %d, %d, %d\n", game->map->floor->r, game->map->floor->g, game->map->floor->b);
