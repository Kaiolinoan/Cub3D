/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klino-an <klino-an@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/16 01:10:31 by kelle             #+#    #+#             */
/*   Updated: 2026/06/02 18:24:13 by klino-an         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

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
		// printf("passou pelo parsing\n");
		// printf("EA: %s\n", (char *)game->sprites.east.path);
		// printf("WE: %s\n", (char *)game->sprites.west.path);
		// printf("SO: %s\n", (char *)game->sprites.south.path);
		// printf("NO: %s\n", (char *)game->sprites.north.path);
		// printf("C: %d, %d, %d\n", game->map->ceiling->r, game->map->ceiling->g, game->map->ceiling->b);
		// printf("F: %d, %d, %d\n", game->map->floor->r, game->map->floor->g, game->map->floor->b);
	// game->debug = true;
	mlx_main(game);
	clear_game(game);
	return (0);
}
