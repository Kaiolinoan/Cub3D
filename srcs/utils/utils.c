#include "cub3d.h"

void	clear_matriz(char **matriz)
{
	size_t	i;

	i = 0;
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

void	clear_game(t_game *game)
{
	free(game->img.east);
	free(game->img.west);
	free(game->img.north);
	free(game->img.south);
	clear_matriz(game->map->grid);
	free(game->map);
	free(game);
}
