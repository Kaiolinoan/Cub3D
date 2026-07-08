/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   clean_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kelle <kelle@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/10 19:12:33 by kelle             #+#    #+#             */
/*   Updated: 2026/07/02 19:27:53 by kelle            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

void	clear_images(t_game *game)
{
	if (game->buffer.img)
		mlx_destroy_image(game->mlx, game->buffer.img);
	if (game->sprites.north.img.img)
		mlx_destroy_image(game->mlx, game->sprites.north.img.img);
	if (game->sprites.east.img.img)
		mlx_destroy_image(game->mlx, game->sprites.east.img.img);
	if (game->sprites.west.img.img)
		mlx_destroy_image(game->mlx, game->sprites.west.img.img);
	if (game->sprites.south.img.img)
		mlx_destroy_image(game->mlx, game->sprites.south.img.img);
	if (game->sprites.black_square.img.img)
		mlx_destroy_image(game->mlx, game->sprites.black_square.img.img);
	if (game->sprites.white_square.img.img)
		mlx_destroy_image(game->mlx, game->sprites.white_square.img.img);
	if (game->sprites.door_frames)
		clear_door(game, 0);
}

void	free_sprites_path(t_game *game)
{
	free(game->sprites.east.path);
	free(game->sprites.west.path);
	free(game->sprites.north.path);
	free(game->sprites.south.path);
	free(game->sprites.black_square.path);
	free(game->sprites.white_square.path);
}

void	clear_game(t_game *game)
{
	if (!game)
		return ;
	clear_images(game);
	free_sprites_path(game);
	if (game->map)
	{
		free(game->map->floor);
		free(game->map->ceiling);
		clear_matriz(game->map->grid);
		free(game->map);
	}
	if (game->door)
		free(game->door);
	if (game->win)
		mlx_destroy_window(game->mlx, game->win);
	if (game->mlx)
		(mlx_destroy_display(game->mlx), free(game->mlx));
	free(game);
}

void	clear_matriz(char **matriz)
{
	size_t	i;

	i = 0;
	if (!matriz)
		return ;
	while (matriz[i])
	{
		free(matriz[i]);
		matriz[i] = NULL;
		i++;
	}
	free(matriz);
	matriz = NULL;
}
