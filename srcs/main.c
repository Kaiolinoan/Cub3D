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

int draw_plain(t_game *game)
{
	t_img img;

	img.img = mlx_new_image(game->mlx, game->win_w, game->win_h);
	if (!img.img)
		return (print_error(MLX_IMG), 0);
	img.addr = mlx_get_data_addr(img.img, &img.bits_per_pixel, &img.line_length, &img.endian);
	if (!img.addr)
		return (print_error(MLX_ADDR), 0);
	int y = 0;
	int x = 0;
	while (y < game->win_h)
	{
		x = 0;
		while (x < game->win_w)
		{
			my_mlx_pixel_put(&img, x, y, 0x00666666);
			x++;
		}
		y++;
	}
	mlx_put_image_to_window(game->mlx, game->win, img.img, 0, 0);
	return (0);
}

void handle_wall(t_game *game, )
{
	t_img sprite;
	void *ptr;
	int size;

	size = 64;
	sprite.img = mlx_xpm_file_to_image(game->mlx, "assets/square-64.xpm", &size, &size);
	if (!ptr)
		return (print_error(MLX_IMG));
	sprite.addr = mlx_get_data_addr(sprite.img, &sprite.bits_per_pixel, &sprite.line_length, &sprite.endian);
	if (!sprite.img)
		print_error(MLX_ADDR);
}

void sla(t_game *game, size_t x, size_t y)
{
	(void)x;
	(void)y;
	if (game->map->grid[y][x] == '1')
		handle_wall(game);
	// mlx_put_image_to_window(game->mlx, game->win, ptr, x * 64, y * 64);
}

int render(t_game *game)
{
	size_t i;
	size_t j;

	i = 0;
	draw_plain(game);
	while (game->map->grid[i])
	{
		j = 0;
		while (game->map->grid[i][j])
		{
			sla(game, j, i);
			j++;
		}
		i++;
	}
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
