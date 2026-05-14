#include "cub3d.h"

static int	check_maze_chars(char **grid)
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

bool	store_maze(t_game *game, char *full_line)
{
	char	**temp;
	int		size;
	int		i;

	if (!check_full_line(full_line))
		return (print_error("There is no map"), free(full_line), false);
	if (!check_empty_line_on_map(full_line))
		return (print_error("Empty line on map"), free(full_line), false);
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
		if (!game->map->grid[i])
			return (print_error(ALLOC_ERR), clear_matriz(game->map->grid), 0);
		i++;
	}
	game->map->grid[i] = NULL;
	return (free(full_line), clear_matriz(temp), true);
}

static void get_starting_dir(int x, int y, t_map *map)
{
	char **grid;

	grid = map->grid;
	if (grid[x][y] == 'N')
		map->starting_direction = NORTH;
	else if (grid[x][y] == 'W')
		map->starting_direction = WEST;
	else if (grid[x][y] == 'E')
		map->starting_direction = EAST;
	else if (grid[x][y] == 'S')
		map->starting_direction = SOUTH;
}
static void get_starting_info(t_game *game)
{
	int i;
	int j;
	char **grid;

	i = 0;
	grid = game->map->grid;
	while (grid[i])
	{
		j = 0;
		while (grid[i][j])
		{
			if (grid[i][j] == 'N' || grid[i][j] == 'S' || grid[i][j] == 'W' || grid[i][j] == 'E')
			{
				game->map->start_x = j;
				game->map->start_y = i;
				get_starting_dir(i, j, game->map);
			}
			j++;
		}
		i++;
	}
}

bool	check_if_map_is_valid(char *filename, t_game *game)
{
	int	chars_nb;

	if (!check_extension(filename, ".cub"))
		return (print_error("Invalid file"), 0);
	chars_nb = check_maze_chars(game->map->grid);
	if (chars_nb < 0)
		return (print_error("Invalid character on map"), 0);
	else if (chars_nb == 0)
		return (print_error("There is no player starting position on map"), 0);
	else if (chars_nb > 1)
		return (print_error("Multiple player's starting position on map"), 0);
	get_starting_info(game);
	if (!flood_fill_prep(game->map))
		return (print_error("Map is not surrounded by walls"), 0);
	return (true);
}
