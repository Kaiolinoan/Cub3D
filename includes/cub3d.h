/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cub3d.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klino-an <klino-an@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/16 01:10:41 by kelle             #+#    #+#             */
/*   Updated: 2026/04/16 18:30:49 by klino-an         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CUB3D_H
# define CUB3D_H

# include "../libs/Libft/includes/libft.h"
# include <X11/X.h>
# include <X11/keysym.h>
# include <mlx.h>
# include <stdbool.h>
# include <time.h>
# include <errno.h> 

# define ALLOC_ERR "Memory allocation failed"
# define RGB_DIGIT_ERR "RGB must only have digits"
# define RGB_RANGE_ERR "RGB values must be between 0 to 255 range"

typedef void*	t_sprite;

typedef enum e_direction
{
	NORTH,
	SOUTH,
	WEST,
	EAST,
}	t_direction;

typedef struct s_rgb
{
	unsigned char	r;
	unsigned char	g;
	unsigned char	b;
}	t_rgb;

typedef struct s_map
{
	char		**grid;
	t_rgb		*floor;
	t_rgb		*ceiling;
	t_direction	starting_direction;
	int			start_x;
	int			start_y;
}	t_map;

typedef struct s_image
{
	t_sprite	north;
	t_sprite	south;
	t_sprite	west;
	t_sprite	east;
}	t_image;

typedef struct s_game
{
	t_image		img;
	void		*mlx;
	void		*win;
	t_map		*map;
}	t_game;

typedef struct s_elem
{
    char *id;
    char **target;
} t_elem;

//--------------------------PARSING-----------------------
bool	check_extension(char *filename, char *extension);
bool	check_if_map_is_valid(char *filename, t_game *game);
int		get_map_details(t_game *game, char *filename);
int		filter_rgb(char **arr, t_game *game);

//ver se realmente tem de ficar aqui no header ou se e so static
bool	check_maze(t_game *game);
bool	store_maze(t_game *game, char *full_line);
bool	flood_fill_prep(t_map *map);


//--------------------------UTILS-----------------------
void	clear_matriz(char **matriz);
void	print_error(char *msg);
void	clear_game(t_game *game);
size_t	array_len(char **array);
int		ft_strcmp(char *s1, char *s2);
bool	check_full_line(char *full_line);
char	*ft_strjoin_and_free(char *s1, char *s2);
bool	check_empty_line_on_map(char *full_line);
bool	check_extension(char *filename, char *extension);


#endif
