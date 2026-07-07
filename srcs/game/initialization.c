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

static char	*build_frame_path(int i)
{
	char	*index_str;
	char	*path;

	index_str = ft_itoa(i);
	if (!index_str)
		return (NULL);
	path = ft_strjoin_and_free(ft_strdup("assets/animation/door_"),
			ft_strjoin_and_free(index_str, ft_strdup(".xpm")));
	return (path);
}

static bool	load_door_frame(t_game *game, t_texture *frame, int i)
{
	char	*path;

	path = build_frame_path(i);
	if (!path)
		return (print_error(ALLOC_ERR), false);
	if (!set_image_path(frame, path))
		return (free(path), false);
	free(path);
	if (!file_to_image(game->mlx, frame))
		return (false);
	if (!get_dir_img_address(frame))
		return (mlx_destroy_image(game->mlx, frame->img.img),
			frame->img.img = NULL, false);
	return (true);
}

static bool	load_door_frames(t_game *game, t_sprites *sprites)
{
	int	i;

	sprites->door_frames = ft_calloc(DOOR_ANIM_FRAMES, sizeof(t_texture));
	if (!sprites->door_frames)
		return (print_error(ALLOC_ERR), false);
	i = 0;
	while (i < DOOR_ANIM_FRAMES)
	{
		if (!load_door_frame(game, &sprites->door_frames[i], i))
		{
			clear_door_frames(game->mlx, sprites->door_frames, i);
			sprites->door_frames = NULL;
			return (false);
		}
		i++;
	}
	return (true);
}

bool	init_path(t_sprites *sprites)
{
	if (!set_image_path(&sprites->black_square, "assets/black_square-64.xpm"))
		return (false);
	if (!set_image_path(&sprites->white_square, "assets/square-64.xpm"))
		return (false);
	if (!set_image_path(&sprites->test1, "assets/test1.xpm"))
		return (false);
	if (!set_image_path(&sprites->test2, "assets/grey_yellow.xpm"))
		return (false);
	if (!set_image_path(&sprites->test3, "assets/grey_pink.xpm"))
		return (false);
	if (!set_image_path(&sprites->test4, "assets/gemini.xpm"))
		return (false);
	if (!set_image_path(&sprites->test5, "assets/test.xpm"))
		return (false);
	if (!set_image_path(&sprites->test6, "assets/test2.xpm"))
		return (false);
	if (!set_image_path(&sprites->test7, "assets/bright.xpm"))
		return (false);
	if (!set_image_path(&sprites->test8, "assets/brick.xpm"))
		return (false);
	if (!set_image_path(&sprites->test9, "assets/door.xpm"))
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
	if (!file_to_image(game->mlx, &sprites->test1))
		return (false);
	if (!file_to_image(game->mlx, &sprites->test2))
		return (false);
	if (!file_to_image(game->mlx, &sprites->test3))
		return (false);
	if (!file_to_image(game->mlx, &sprites->test4))
		return (false);
	if (!file_to_image(game->mlx, &sprites->test5))
		return (false);
	if (!file_to_image(game->mlx, &sprites->test6))
		return (false);
	if (!file_to_image(game->mlx, &sprites->test7))
		return (false);
	if (!file_to_image(game->mlx, &sprites->test8))
		return (false);
	if (!file_to_image(game->mlx, &sprites->test9))
		return (false);
	return (true);
}
	// file_to_image(game->mlx, &sprites->test);
	// file_to_image(game->mlx, &sprites->test8);
	// file_to_image(game->mlx, &sprites->test9);
	// file_to_image(game->mlx, &sprites->test10);

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
	if (!get_dir_img_address(&sprites->test1))
		return (false);
	if (!get_dir_img_address(&sprites->test2))
		return (false);
	if (!get_dir_img_address(&sprites->test3))
		return (false);
	if (!get_dir_img_address(&sprites->test4))
		return (false);
	if (!get_dir_img_address(&sprites->test5))
		return (false);
	if (!get_dir_img_address(&sprites->test6))
		return (false);
	if (!get_dir_img_address(&sprites->test7))
		return (false);
	if (!get_dir_img_address(&sprites->test8))
		return (false);
	if (!get_dir_img_address(&sprites->test9))
		return (false);
	return (true);
}

// get_dir_img_address(&sprites->test);
// get_dir_img_address(&sprites->test8);
// get_dir_img_address(&sprites->test9);
// get_dir_img_address(&sprites->test10);
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
