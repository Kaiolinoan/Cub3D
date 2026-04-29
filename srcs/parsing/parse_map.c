#include "cub3d.h"

bool	check_maze_borders(char **grid)
{
	size_t	i;
	size_t	j;
	size_t	imx;
	size_t	jmx;

	i = 0;
	imx = array_len(grid);
	while (grid[i])
	{
		j = 0;
		jmx = ft_strlen(grid[i]);
		while (grid[i][j])
		{
			if ((i == 0 && grid[i][j] != '1' && grid[i][j])
				|| (i == imx - 1 && grid[i][j] != '1' && grid[i][j]))
				return (false);
			if ((j == 0 && grid[i][j] != '1')
				|| (j == jmx - 1 && grid[i][j] != '1'))
				return (false);
			j++;
		}
		i++;
	}
	return (true);
}

int	check_maze_chars(char **grid)
{
	size_t	i;
	size_t	j;
	int		count;

	i = 0;
	count = 0;
	while (grid[i])
	{
		j = 0;
		while (grid[i][j])
		{
			if (grid[i][j] && grid[i][j] != '0' && grid[i][j] != '1' && grid[i][j] != ' ')
			{
				if (grid[i][j] == 'N' || grid[i][j] == 'S'
					|| grid[i][j] == 'W' || grid[i][j] == 'E')
					count++;
				else
					return (-1);
			}
			j++;
		}
		i++;
	}
	return (count);
}

bool check_full_line(char *full_line)
{
	int	i;

	i = 0;
	if (!full_line)
		return (false);
	while (full_line[i])
	{
		if (full_line[i] != '\n')
			return (true);
		i++;
	}
	return (false);
}

bool	store_maze(t_game *game, char *full_line)
{
	char	**temp;
	int		size;
	int		i;

	if (!check_full_line(full_line))
		return (print_error("There is no map"), free(full_line), false);
	temp = ft_split(full_line, '\n');
	if (!temp || !*temp)
		return (free(full_line), false);
	size = array_len(temp);
	game->map->grid = ft_calloc(size + 1, sizeof(char *));
	if (!game->map->grid)
		return (free(full_line), clear_matriz(temp), print_error(ALLOC_ERR), 0);
	i = 0;
	while (temp[i])
	{
		game->map->grid[i] = ft_strdup(temp[i]);
		i++;
	}
	game->map->grid[i] = NULL;
	return (free(full_line), clear_matriz(temp), true);
}
