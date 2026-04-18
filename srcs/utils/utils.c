#include "cub3d.h"

void clear_matriz(char **matriz)
{
    size_t i;

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
	ft_putstr_fd(header, 2);
	ft_putstr_fd(msg, 2);
	write(2, "\n", 1);
}

void clear_game(t_game *game)
{
    free(game->img.east);
    free(game->img.west);
    free(game->img.north);
    free(game->img.south);
    free(game->map);
    free(game);
}