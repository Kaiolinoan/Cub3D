#include "cub3d.h"

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
void get_starting_info(t_game *game)
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

char *get_validated_full_line(char *full_line)
{
	char *new;

	new = ft_strtrim(full_line, "\n");
	free(full_line);
	if (!new)
		return (print_error(ALLOC_ERR), NULL);
	if (!check_full_line(new))
		return (print_error("There is no map"), free(new), NULL);
	if (!check_empty_line_on_map(new))
		return (print_error("Empty line on map"), free(new), NULL);
	return (new);
}