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
	t_rgb		floor;
	t_rgb		ceiling;
	t_direction	starting_direction;
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

//--------------------------PARSING-----------------------
bool	check_map_extension(char *filename);
bool	check_if_map_is_valid(char *filename, t_game *game);
int		get_map_details(t_game *game, char *filename);
//ver se realmente tem de ficar aqui no header ou se e so static
bool	check_maze_borders(char **grid);
bool	check_maze(t_game *game);
int		check_maze_chars(char **grid);
bool	store_maze(t_game *game, char *full_line);

//--------------------------UTILS-----------------------
void	clear_matriz(char **matriz);
void	print_error(char *msg);
void	clear_game(t_game *game);
size_t	array_len(char **array);
size_t	ft_strlen_without_space(const char *s);
char	*ft_strjoin3(char *s1, char *s2); //mudar esse nome dps

#endif
