#include "cub3d.h"

void clear_images2(t_game *game)
{
	if (game->sprites.test.img.img)
		mlx_destroy_image(game->mlx, game->sprites.test.img.img);
	if (game->sprites.test1.img.img)
		mlx_destroy_image(game->mlx, game->sprites.test1.img.img);
	if (game->sprites.test2.img.img)
		mlx_destroy_image(game->mlx, game->sprites.test2.img.img);
	if (game->sprites.test3.img.img)
		mlx_destroy_image(game->mlx, game->sprites.test3.img.img);
	if (game->sprites.test4.img.img)
		mlx_destroy_image(game->mlx, game->sprites.test4.img.img);
	if (game->sprites.test5.img.img)
		mlx_destroy_image(game->mlx, game->sprites.test5.img.img);
	if (game->sprites.test6.img.img)
		mlx_destroy_image(game->mlx, game->sprites.test6.img.img);
	if (game->sprites.test7.img.img)
		mlx_destroy_image(game->mlx, game->sprites.test7.img.img);
	if (game->sprites.test8.img.img)
		mlx_destroy_image(game->mlx, game->sprites.test8.img.img);
	if (game->sprites.test9.img.img)
		mlx_destroy_image(game->mlx, game->sprites.test9.img.img);
	if (game->sprites.test10.img.img)
		mlx_destroy_image(game->mlx, game->sprites.test10.img.img);
}

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
	clear_images2(game);
}

void	free_sprites_path(t_game *game)
{
	free(game->sprites.east.path);
	free(game->sprites.west.path);
	free(game->sprites.north.path);
	free(game->sprites.south.path);
	free(game->sprites.black_square.path);
	free(game->sprites.white_square.path);
	free(game->sprites.test.path);
	free(game->sprites.test1.path);
	free(game->sprites.test2.path);
	free(game->sprites.test3.path);
	free(game->sprites.test4.path);
	free(game->sprites.test5.path);
	free(game->sprites.test6.path);
	free(game->sprites.test7.path);
	free(game->sprites.test8.path);
	free(game->sprites.test9.path);
	free(game->sprites.test10.path);
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
