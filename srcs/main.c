/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cub3d.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klino-an <klino-an@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/16 01:10:31 by kelle             #+#    #+#             */
/*   Updated: 2026/04/16 18:11:15 by klino-an         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

t_game	*initialize_game(char *filename)
{
	t_game	*game;

	game = ft_calloc(1, sizeof(t_game));
	game->map = ft_calloc(1, sizeof(t_map));
	if (!game || !game->map)
		return (clear_game(game), print_error(ALLOC_ERR), NULL);
	game->map->ceiling = NULL;
	game->map->floor = NULL;
	game->img.east = NULL;
	game->img.south = NULL;
	game->img.north = NULL;
	game->img.west = NULL;
	game->map->grid = NULL;
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
	clear_game(game);
	printf("passou pelo parsing\n");
}
