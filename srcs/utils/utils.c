#include "cub3d.h"

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

void	print_error(char *msg)
{
	char	*header;

	header = "\033[1;31mError\033[0m\n";
	ft_dprintf(2, "%s%s\n", header, msg);
}

size_t	array_len(char **array)
{
	size_t	i;

	i = 0;
	if (!array)
		return (0);
	while (array[i])
		i++;
	return (i);
}

void clear_images(t_game *game)
{
	mlx_destroy_image(game->mlx, game->buffer.img);
	mlx_destroy_image(game->mlx, game->sprites.east.img.img);
	mlx_destroy_image(game->mlx, game->sprites.west.img.img);
	mlx_destroy_image(game->mlx, game->sprites.north.img.img);
	mlx_destroy_image(game->mlx, game->sprites.south.img.img);
}

void	clear_game(t_game *game)
{
	if (!game)
		return;
	clear_images(game);
	free(game->sprites.east.path);
	free(game->sprites.west.path);
	free(game->sprites.north.path);
	free(game->sprites.south.path);
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

void remove_new_line_in_array(char **arr)
{
	size_t i;
	size_t j;

	i = 0;
	while (arr[i])
	{
		j = 0;
		while(arr[i][j])
		{
			if (arr[i][j] == '\n')
				arr[i][j] = '\0';
			j++;
		}
		i++;
	}
}
void remove_new_line(char *str)
{
	size_t i;

	i = 0;
	while (str[i])
	{
		if (str[i] == '\n')
			str[i] = '\0';
		i++;
	}
}