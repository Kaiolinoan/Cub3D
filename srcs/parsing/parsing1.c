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

int get_map_sprites(t_game *game, int count)
{
    if (count == 0)
    
    return (1);
}

int get_map_details(t_game *game, char *filename)
{
    int fd;
    int count;
    char    *line;

    fd = open(filename, O_RDONLY);
    if (fd < 0)
        return (0);
    count = 0;
    while (1)
    {
        line = get_next_line(fd);
        if (!line)
            break ;
        if (!get_map_sprites(game, count))
            return (free(line), 0);
        count++;
    }
    
    return (1);
}