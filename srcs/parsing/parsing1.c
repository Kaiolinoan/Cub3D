#include "cub3d.h"

bool	check_map_extension(char *filename)
{
	char	*extension;

	extension = ft_strrchr(filename, '.');
	if (!extension)
		return (false);
	if (!ft_strncmp(extension, ".cub", 5))
		return (true);
	return (false);
}

bool	check_if_map_is_valid(char *filename, t_game *game)
{
	int	chars_nb;

	if (!check_map_extension(filename))
		return (print_error("Invalid file"), 0);
	chars_nb = check_maze_chars(game->map->grid);
	if (chars_nb < 0)
		return (print_error("Invalid character on map"), 0);
	else if (chars_nb == 0)
		return (print_error("There is no player starting position on map"), 0);
	else if (chars_nb > 1)
		return (print_error("Multiple player's starting position on map"), 0);
	if (!check_maze_borders(game->map->grid))
		return (print_error("Map is not surrounded by walls"), 0);
	return (true);
}

static int	get_file_fd(char *filename)
{
	int	fd;

	fd = open(filename, O_RDONLY);
	if (fd < 0)
	{
		if (errno == EACCES)
			print_error("Permission denied");
		else if (errno == ENOENT)
			print_error("File does not exist");
		return (-1);
	}
	return (fd);
}

bool	fill_rgb(t_game *game, char **rgb, bool is_floor)
{
	if (is_floor)
	{
		game->map->floor = ft_calloc(1, sizeof(t_rgb));
		if (!game->map->floor)
			return (print_error(ALLOC_ERR), false);
		game->map->floor->r = (unsigned char)ft_atoi(rgb[0]);
		game->map->floor->g = (unsigned char)ft_atoi(rgb[1]);
		game->map->floor->b = (unsigned char)ft_atoi(rgb[2]);
	}
	else
	{
		game->map->ceiling = ft_calloc(1, sizeof(t_rgb));
		if (!game->map->ceiling)
			return (print_error(ALLOC_ERR), false);
		game->map->ceiling->r = (unsigned char)ft_atoi(rgb[0]);
		game->map->ceiling->g = (unsigned char)ft_atoi(rgb[1]);
		game->map->ceiling->b = (unsigned char)ft_atoi(rgb[2]);
	}
	return (true);
}

static bool	parse_and_fill_rgb(t_game *game, char **coord, bool is_floor)
{
	char	**rgb;
	char	*str;
	size_t	i;
	size_t	j;

	str = ft_strtrim(coord[1], "\n");
	if (!str)
		return(print_error(ALLOC_ERR), false);
	rgb = ft_split(str, ',');
	free(str);
	if (!rgb || !*rgb || !rgb[1] || !rgb[2] || rgb[3])
		return (print_error("Invalid RGB"), clear_matriz(rgb), false);
	i = -1;
	while (rgb[++i])
	{
		j = -1;
		while (rgb[i][++j])
			if (!ft_isdigit(rgb[i][j]) && rgb[i][j])
				return (print_error(RGB_DIGIT_ERR), clear_matriz(rgb), false);
		if (ft_atoi(rgb[i]) < 0 || ft_atoi(rgb[i]) > 255 || *rgb[i] == '\n')
			return (print_error(RGB_RANGE_ERR), clear_matriz(rgb), false);
	}
	if (!fill_rgb(game, rgb, is_floor))
		return (clear_matriz(rgb), false);
	return (clear_matriz(rgb), true);
}

int sla_rgb(char **arr, t_game *game)
{
	if (!ft_strncmp(*arr, "C", 2))
	{
		if (!game->map->ceiling)
		{
			if (parse_and_fill_rgb(game, arr, false))
				return (1);
			return (-2);
		}
		return (-1);
	}
	if (!ft_strncmp(*arr, "F", 2))
	{
		if (!game->map->floor)
		{
			if (parse_and_fill_rgb(game, arr, true))
				return (1);
			return (-2);
		}
		return (-1);
	}
	return (0);
}
int sla(char **arr, t_game *game)
{
	if (!ft_strncmp(*arr, "NO", 3))
	{
		if (!game->img.north)
			return (game->img.north = ft_strdup(arr[1]), 1);
		return (-1);
	}
	if (!ft_strncmp(*arr, "SO", 3))
	{
		if (!game->img.south)
			return (game->img.south = ft_strdup(arr[1]), 1);
		return (-1);
	}
	if (!ft_strncmp(*arr, "WE", 3))
	{
		if (!game->img.west)
			return (game->img.west = ft_strdup(arr[1]), 1);
		return (-1);
	}
	if (!ft_strncmp(*arr, "EA", 3))
	{
		if (!game->img.east)
			return (game->img.east = ft_strdup(arr[1]), 1);
		return (-1);
	}
	return(sla_rgb(arr, game));
}

static int	check_and_assign_coordinate(t_game *g, char *line)
{
	int tmp;
	static int count = 0;
	char		**arr;

	// if (!line)
	// 	return (-2);
	if (count == 6)
		return (count);
	if (line && line[0] == '\n')
		return (0);
	arr = ft_split(line, ' ');
	if (!arr || !*arr || !arr[1])
		return (clear_matriz(arr), -1);
	tmp = sla(arr, g);
	clear_matriz(arr);
	if (tmp == -1)
		return (101);
	else if (tmp == -2)
		return (-3);
	else if (tmp == 1 && count < 6)
		return (count++);
	return (count);
}
int	get_map_details(t_game *game, char *filename)
{
	int		count;
	char	*line;
	char	*full_line;
	int		fd;

	full_line = NULL;
	fd = get_file_fd(filename);
	if (fd < 0)
		return (false);
	while (1)
	{
		line = get_next_line(fd);
		count = check_and_assign_coordinate(game, line);
		// printf ("%d\n", count);
		if (!line || count < 0 || count > 6)
			break ;
		if (count == 6)
			full_line = ft_strjoin3(full_line, line);
		free(line);
	}
	if (count == 6)
	{
		if (!store_maze(game, full_line))
			return (close(fd), free(line), false);
	}
	else
	{
		if (count == -1)
			print_error("Invalid element");
		else if (count == -2)
			print_error("Empty map");
		else if (count > 6)
			print_error("Double definition of element");
	}
	if (count < 0 || count > 6)
		return (close(fd), free(line), false);
	return (close(fd), free(line), true);
}
