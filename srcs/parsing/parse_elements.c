#include "cub3d.h"

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

static t_elem *init_elements(t_game *game)
{

	static t_elem elements[5];

	elements[0].id = "NO";
	elements[0].target = (char **)&game->sprites.north.path;
	elements[1].id = "SO";
 	elements[1].target = (char **)&game->sprites.south.path;
	elements[2].id = "WE";
	elements[2].target = (char **)&game->sprites.west.path;
	elements[3].id = "EA";
	elements[3].target = (char **)&game->sprites.east.path;
	elements[4].id = NULL;
	elements[4].target = NULL;

	return (elements);
}

static int filter_element(char **arr, t_game *game)
{
	t_elem *elements;
	int i;

	i= 0;
	elements = init_elements(game);
	while (elements[i].id)
	{
		if (!ft_strncmp(*arr, elements[i].id, 3))
		{
			if (*(elements[i].target))
				return (print_error("Double definition of element"), -1);
			else if (!check_extension(arr[1], ".xpm"))
				return (print_error("Element has an invalid extension") ,-2);
			return (*(elements[i].target) = ft_strdup(arr[1]), 1);
		}
		i++;
	}
	return (filter_rgb(arr, game));
}

static int	check_and_assign_coordinate(t_game *g, char *line)
{
	int tmp;
	static int count = 0;
	char		**arr;

	if (count == 6)
		return (count);
	if (!line)
		return (print_error("Empty file"), -2);
	if (line && line[0] == '\n')
		return (0);
	arr = ft_split(line, ' ');
	if (!arr || !*arr || !arr[1])
		return (clear_matriz(arr), print_error("Invalid element"), -1);
	remove_new_line(arr);
	tmp = filter_element(arr, g);
	clear_matriz(arr);
	if (tmp < 0)
		return (-1);
	else if (tmp && count < 6)
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
		if (!line || count < 0 || count > 6)
			break ;
		if (count == 6)
			full_line = ft_strjoin_and_free(full_line, line);
		free(line);
	}
	if (count > 0 && count < 6)
		print_error("Fewer elements than expexted");
	if (((count == 6) && (!store_maze(game, full_line))) || (count != 6))
		return (close(fd), free(line), false);
	return (close(fd), free(line), true);
}
