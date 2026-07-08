/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   initialization.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kelle <kelle@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/10 19:11:50 by kelle             #+#    #+#             */
/*   Updated: 2026/07/06 03:29:22 by kelle            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

bool	init_path(t_sprites *sprites)
{
	if (!set_image_path(&sprites->black_square, "assets/black_square-64.xpm"))
		return (false);
	if (!set_image_path(&sprites->white_square, "assets/square-64.xpm"))
		return (false);
	return (true);
}

bool	init_images(t_game *game, t_sprites *sprites)
{
	if (!file_to_image(game->mlx, &sprites->east))
		return (false);
	if (!file_to_image(game->mlx, &sprites->west))
		return (false);
	if (!file_to_image(game->mlx, &sprites->north))
		return (false);
	if (!file_to_image(game->mlx, &sprites->south))
		return (false);
	if (!file_to_image(game->mlx, &sprites->white_square))
		return (false);
	if (!file_to_image(game->mlx, &sprites->black_square))
		return (false);
	return (true);
}

bool	init_address(t_sprites *sprites)
{
	if (!get_dir_img_address(&sprites->east))
		return (false);
	if (!get_dir_img_address(&sprites->west))
		return (false);
	if (!get_dir_img_address(&sprites->north))
		return (false);
	if (!get_dir_img_address(&sprites->south))
		return (false);
	if (!get_dir_img_address(&sprites->white_square))
		return (false);
	if (!get_dir_img_address(&sprites->black_square))
		return (false);
	return (true);
}

bool	initialize_images(t_game *game, t_sprites *sprites)
{
	game->buffer.img = mlx_new_image(game->mlx, game->win_w, game->win_h);
	if (!game->buffer.img)
		return (print_error(MLX_IMG), false);
	game->buffer.addr = mlx_get_data_addr(game->buffer.img,
			&game->buffer.bits_per_pixel, &game->buffer.line_length,
			&game->buffer.endian);
	if (!game->buffer.addr)
		return (print_error(MLX_ADDR), false);
	if (!init_path(sprites))
		return (false);
	if (!init_images(game, sprites))
		return (false);
	if (!init_address(sprites))
		return (false);
	if (!load_door_frames(game, sprites))
		return (false);
	return (true);
}

t_game	*initialize_game(char *filename)
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
	game->minimap = true;
	game->minimap_tile_size = 20;
	game->minimap_radius_tiles = 7.0;
	game->movement_speed = 0.07;
	game->rotation_speed = 0.05;
	game->fov = 0.66;
	return (game);
}
