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

bool check_if_map_is_valid(char *filename, t_game *game)
{
    int chars_nb;

    if (!check_map_extension(filename))
        return (print_error("Invalid file"), false);
    chars_nb = check_maze_chars(game->map->grid);
    if (chars_nb < 0)
        return (print_error("Invalid character on map"), false);
    else if (chars_nb == 0)
        return (print_error("There is no player starting position on map"), false);
    else if (chars_nb > 1)
        return (print_error("Multiple player's starting position on map"), false);
    if (!check_maze_borders(game->map->grid))
        return (print_error("Map is not surrounded by walls"), false);
    return (true);
}

static int get_file_fd(char *filename)
{
    int fd;

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

void fill_rgb(t_game *game, char **rgb, bool is_floor)
{
    if (is_floor)
    {
        game->map->floor.r = (unsigned char)ft_atoi(rgb[0]);
        game->map->floor.g = (unsigned char)ft_atoi(rgb[1]); 
        game->map->floor.b = (unsigned char)ft_atoi(rgb[2]);
    }
    else
    {
        game->map->ceiling.r = (unsigned char)ft_atoi(rgb[0]);
        game->map->ceiling.g = (unsigned char)ft_atoi(rgb[1]);
        game->map->ceiling.b = (unsigned char)ft_atoi(rgb[2]);
    }
}

static bool parse_and_fill_rgb(t_game *game, char **coord, bool is_floor)
{
    char **rgb;
    size_t i;
    size_t j;

    rgb = ft_split(coord[1] , ',');
    if (!rgb || !*rgb || !rgb[1] ||  !rgb[2] || rgb[3])
        return (print_error("Invalid RGB"), clear_matriz(rgb), false);
    i = 0;
    while (rgb[i])
    {
        j = 0;
        while(rgb[i][j])
            if (!ft_isdigit(rgb[i][j++]) && rgb[i][j])
                return (print_error("Invalid RGB"), clear_matriz(rgb), 0);
        if (ft_atoi(rgb[i]) < 0 || ft_atoi(rgb[i]) > 255 || *rgb[i] == '\n')
            return (print_error("Invalid RGB"), clear_matriz(rgb), false);    
        i++;
    }
    return (fill_rgb(game, rgb, is_floor), clear_matriz(rgb), true);
}

static int check_and_assign_coordinate(t_game *game, char *line)
{
    static int count;
    char **coord;

    if (count == 6)
        return (count);
    if (!line)
        return (-1);
    if (line && line[0] == '\n')
        return (0);
    coord = ft_split(line, ' ');
    if (!coord || !*coord || !coord[1])
        return (print_error("Invalid coordinate"), clear_matriz(coord), -1);
    if (!ft_strncmp(*coord, "NO", 3))
        return (count++, game->img.north = ft_strdup(coord[1]), clear_matriz(coord), 1);
    else if (!ft_strncmp(*coord, "SO", 3))
        return (count++, game->img.south = ft_strdup(coord[1]), clear_matriz(coord), 1);
    else if (!ft_strncmp(*coord, "WE", 3))
        return (count++, game->img.west = ft_strdup(coord[1]), clear_matriz(coord), 1);
    else if (!ft_strncmp(*coord, "EA", 3))
        return (count++, game->img.east = ft_strdup(coord[1]), clear_matriz(coord), 1);
    else if (!ft_strncmp(*coord, "F", 2))
        return (count++, parse_and_fill_rgb(game, coord, true), clear_matriz(coord), 1);
    else if (!ft_strncmp(*coord, "C", 2)) 
        return (count++, parse_and_fill_rgb(game, coord, false), clear_matriz(coord), 1);
    return (0);
}

int get_map_details(t_game *game, char *filename)
{
    int     count;
    char    *line;
    char    *full_line;
    int     fd;

    full_line = NULL;
    fd = get_file_fd(filename);
    if (fd < 0)
        return(false);
    while (1)
    {
        line = get_next_line(fd);
        count = check_and_assign_coordinate(game, line);
        if (!line || count < 0)
            break;
        if (count == 6)// dar free full line;
            full_line = ft_strjoin3(full_line, line);
        free(line);
    }
    if (count == 6)
        if (!store_maze(game, full_line))
            return (close(fd), free(line), false);
    return (close(fd), free(line), true);
}
