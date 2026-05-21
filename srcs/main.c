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

static int key_inputs(int keycode, void *param)
{
	t_game *game = param;
	if (keycode == XK_Escape)
	{
		clear_game(game);
		exit(EXIT_SUCCESS);
	}
	return (0);
}

static int finish_game(void *param)
{
	t_game *game = param;
	clear_game(game);
	exit(EXIT_SUCCESS);
	return (0);
}

void my_mlx_pixel_put(t_img *img, int x, int y, int color)
{
	char *dst;
	int offset;

	offset = (y * img->line_length + x * (img->bits_per_pixel / 8));
	dst = img->addr + offset;
	*(unsigned int *)dst = color;
}
void draw_texture(t_img *img, t_img sprite, int x, int y)
{
	int size;
	int tex_y;
	int tex_x;

	tex_y= 0;
	size = PX;
	while (tex_y < size)
	{
		tex_x = 0;
		while (tex_x < size)
		{
			int offset = (tex_y * sprite.line_length) + (tex_x * sprite.bits_per_pixel / 8);
			unsigned int color = *(unsigned int *)(sprite.addr + offset);
			my_mlx_pixel_put(img, tex_x + x, tex_y + y, color);
			tex_x++;
		}
		tex_y++;
	}
}
void prepare_to_draw(t_game *game, t_img *img)
{
	char **map;
	size_t y;
	size_t x;

	y = 0;
	map = game->map->grid;
	while (map[y])
	{
		x = 0;
		while (map[y][x])
		{
			if (map[y][x] == '1')
				draw_texture(img, game->sprites.east.img, x * PX , y * PX);
			else if (map[y][x] == 'N')
				draw_texture(img, game->sprites.south.img, x * PX , y * PX);
			x++;
		}
		y++;
	}
}
void paint_background(t_game *game, t_img *img)
{
	int y;
	int x;

	y = 0;
	while (y < game->win_h)
	{
		x = 0;
		while (x < game->win_w)
		{
			my_mlx_pixel_put(img, x, y, 0x00666666);
			x++;
		}
		y++;
	}
}
int render(t_game *game)
{
	paint_background(game, &game->buffer.img);
	prepare_to_draw(game, &game->buffer.img);
	mlx_put_image_to_window(game->mlx, game->win, game->buffer.img, 0, 0);
	return (0);
}

bool initialize_images(t_game *game)
{
	int size;

	size = PX; //separar esta funcao em duas dps
	game->buffer.img = mlx_new_image(game->mlx, game->win_w, game->win_h);
	game->sprites.east.img.img = mlx_xpm_file_to_image(game->mlx, game->sprites.east.path, &size, &size);
	game->sprites.west.img.img = mlx_xpm_file_to_image(game->mlx, game->sprites.west.path, &size, &size);
	game->sprites.north.img.img = mlx_xpm_file_to_image(game->mlx, game->sprites.north.path, &size, &size);
	game->sprites.south.img.img = mlx_xpm_file_to_image(game->mlx, game->sprites.south.path, &size, &size);
	if (!game->buffer.img || !game->sprites.east.img.img || !game->sprites.west.img.img || !game->sprites.north.img.img || !game->sprites.south.img.img)
		return (print_error(MLX_IMG), 0);
	game->buffer.addr = mlx_get_data_addr(game->buffer.img, &game->buffer.bits_per_pixel, &game->buffer.line_length, &game->buffer.endian);
	game->sprites.east.img.addr = mlx_get_data_addr(game->sprites.east.img.img, &game->sprites.east.img.bits_per_pixel, &game->sprites.east.img.line_length, &game->sprites.east.img.endian);
	game->sprites.west.img.addr = mlx_get_data_addr(game->sprites.west.img.img, &game->sprites.west.img.bits_per_pixel, &game->sprites.west.img.line_length, &game->sprites.west.img.endian);
	game->sprites.north.img.addr = mlx_get_data_addr(game->sprites.north.img.img, &game->sprites.north.img.bits_per_pixel, &game->sprites.north.img.line_length, &game->sprites.north.img.endian);
	game->sprites.south.img.addr = mlx_get_data_addr(game->sprites.south.img.img, &game->sprites.south.img.bits_per_pixel, &game->sprites.south.img.line_length, &game->sprites.south.img.endian);
	if (!game->buffer.addr || !game->sprites.east.img.addr || !game->sprites.west.img.addr || !game->sprites.north.img.addr || !game->sprites.south.img.addr)
		return (print_error(MLX_ADDR)), 0;
	return (1);
}

static void mlx_main(t_game *game)
{
	int screen_w;
	int screen_h;

	game->mlx = mlx_init();
	if (!game->mlx)
		return (print_error(MLX), clear_game(game));
	mlx_get_screen_size(game->mlx, &screen_w, &screen_h);
	// game->win_w = screen_w / 1.5;
	// game->win_h = screen_h / 1.5;
	game->win_w = ft_strlen(*game->map->grid) * 64;
	game->win_h = array_len(game->map->grid) * 64;
	game->win = mlx_new_window(game->mlx, game->win_w, game->win_h, "Cub3D");
	if (!initialize_images(game))
		return ;
	if (!game->win)
		return (print_error(MLX_WIN), clear_game(game));
	mlx_key_hook(game->win, key_inputs, game);
	mlx_hook(game->win, DestroyNotify, StructureNotifyMask, (int (*)())finish_game, game);
	mlx_loop_hook(game->mlx, &render, game);
	mlx_loop(game->mlx);
}

static t_game *initialize_game(char *filename)
{
	t_game *game;

	game = ft_calloc(1, sizeof(t_game));
	if (!game)
		return (print_error(ALLOC_ERR), NULL);
	game->map = ft_calloc(1, sizeof(t_map));
	if (!game->map)
		return (clear_game(game), print_error(ALLOC_ERR), NULL);
	// game->map->ceiling = NULL;
	// game->map->floor = NULL;
	if (!get_map_details(game, filename))
		return (clear_game(game), NULL);
	return (game);
}

int main(int argc, char **argv)
{
	t_game *game;

	if (argc != 2)
		return (print_error("Invalid argument"), 1);
	game = initialize_game(argv[1]);
	if (!game)
		return (1);
	if (!check_if_map_is_valid(argv[1], game))
		return (clear_game(game), 1);
	// printf("passou pelo parsing\n");
	printf("EA: %s\n", (char *)game->sprites.east.path);
	printf("WE: %s\n", (char *)game->sprites.west.path);
	printf("SO: %s\n", (char *)game->sprites.south.path);
	printf("NO: %s\n", (char *)game->sprites.north.path);
	printf("C: %d, %d, %d\n", game->map->ceiling->r, game->map->ceiling->g, game->map->ceiling->b);
	printf("F: %d, %d, %d\n", game->map->floor->r, game->map->floor->g, game->map->floor->b);
	mlx_main(game);
	clear_game(game);
	return (0);
}


