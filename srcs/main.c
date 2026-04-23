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

t_game *initialize_game(char* filename)
{
    t_game *game;

    game = ft_calloc(1, sizeof(t_game));
    game->map = ft_calloc(1, sizeof(t_map));
    if (!game || !game->map)
        return (print_error(ALLOC_ERR), NULL);
    game->map->ceiling.r = 0;
    game->map->ceiling.g = 0;
    game->map->ceiling.b = 0;
    game->map->floor.r = 0;
    game->map->floor.g = 0;
    game->map->floor.b = 0;
    if (!get_map_details(game, filename))
        return (NULL);
    return (game);
}

int main(int argc, char **argv)
{
    t_game *game;

    if (argc != 2)
        return (1);
    game = initialize_game(argv[1]);
    if (!game)
        return (1);
    if (!check_if_map_is_valid(argv[1]))
        return (printf("deu erro\n"), 1);
    // printf("NO: %s\n", (char*)game->img.north);
    // printf("SO: %s\n", (char*)game->img.south);
    // printf("WE: %s\n", (char*)game->img.west);
    // printf("EA: %s\n", (char*)game->img.east);
    // printf("C: %d,%d,%d\n",  game->map->ceiling.r, game->map->ceiling.g,  game->map->ceiling.b);
    // printf("F: %d,%d,%d\n",  game->map->floor.r, game->map->floor.g,  game->map->floor.b);
    clear_game(game);
}