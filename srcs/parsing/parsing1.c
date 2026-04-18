#include "cub3d.h"

bool check_map_extension(char *filename)
{
    char *extension;

    extension = ft_strrchr(filename, '.');
    if (!extension)
        return (false);
    if (!ft_strncmp(extension, ".cub", 5))
        return (true);
    return (false);
}

bool check_if_map_is_valid(char *filename)
{
    if (!check_map_extension(filename))
        return (ft_dprintf(2, "Error\nInvalid file\n"), false);
    return (true);
}

static int get_file_fd(char *filename)
{
    int fd;

    fd = open(filename, O_RDONLY);
    if (fd < 0)
    {
        if (errno == EACCES)
            ft_dprintf(2, "Error\nPermission denied");
        else if (errno == ENOENT)
            ft_dprintf(2, "Error\nFile does not exist");
        return (-1);
    }
    return (fd);
}

void fill_rgb(t_game *game, char **rgb, bool is_floor)
{
    if (is_floor)
    {
        game->map->floor.r = ft_atoi(rgb[0]); 
        game->map->floor.g = ft_atoi(rgb[1]); 
        game->map->floor.b = ft_atoi(rgb[2]); 
    }
    else
    {
        game->map->ceiling.r = ft_atoi(rgb[0]); 
        game->map->ceiling.g = ft_atoi(rgb[1]); 
        game->map->ceiling.b = ft_atoi(rgb[2]); 
    }
}

static bool parse_and_fill_rgb(t_game *game, char **coord, bool is_floor)
{
    char **rgb;
    size_t i;
    size_t j;

    rgb = ft_split(*coord + 1, ',');
    if (!rgb || !*rgb || !rgb[1] ||  !rgb[2] || rgb[3])
        return (print_error("Invalid RGB"), clear_matriz(rgb), false);
    i = 0;
    while (rgb[i])
    {
        j = 0;
        while(rgb[i][j])
            if (!ft_isdigit(rgb[i][j++]))
                return (print_error("Invalid RGB"), clear_matriz(rgb), 0);
        if (ft_atoi(rgb[i]) < 0 || ft_atoi(rgb[i]) > 255) 
            return (print_error("Invalid RGB"), clear_matriz(rgb), false);    
        i++;
    }
    return (fill_rgb(game, rgb, is_floor), clear_matriz(rgb), true);
}

static bool check_and_assign_coordinate(t_game *game, char *line)
{
    char **coord;

    coord = ft_split(line, ' ');
    if (!coord || !*coord || !coord[1])
        return (print_error("Invalid coordinate"), clear_matriz(coord), 0);
    if (!ft_strncmp(*coord, "NO", 3))
        return (game->img.north = ft_strdup(coord[1]), clear_matriz(coord), 1);
    else if (!ft_strncmp(*coord, "SO", 3))
        return (game->img.south = ft_strdup(coord[1]), clear_matriz(coord), 1);
    else if (!ft_strncmp(*coord, "WE", 3))
        return (game->img.west = ft_strdup(coord[1]), clear_matriz(coord), 1);
    else if (!ft_strncmp(*coord, "EA", 3))
        return (game->img.east = ft_strdup(coord[1]), clear_matriz(coord), 1);
    else if (!ft_strncmp(*coord, "F", 2))
        return (parse_and_fill_rgb(game, coord, true), clear_matriz(coord), 1);
    else if (!ft_strncmp(*coord, "C", 2)) 
        return (parse_and_fill_rgb(game, coord, false), clear_matriz(coord), 1);
    return (0);
}

int get_map_details(t_game *game, char *filename)
{
    int     count;
    char    *line;
    int     fd;
    
    count = 0;
    
    fd = get_file_fd(filename);
    if (fd < 0)
        return(0);
    while (1)
    {
        line = get_next_line(fd);
        if (!line)
            break ;
        if (check_and_assign_coordinate(game, line))
            count++;
        if (count == 6)
        {
            // check_maze();
            break;
        }
        free(line);
    }
    return (close(fd), free(line), 1);
}