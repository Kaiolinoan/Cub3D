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

typedef void*	t_sprite;

typedef enum e_direction
{
	NORTH,
	SOUTH,
	WEST,
	EAST,
}	t_direction;

typedef struct	s_map
{
	t_direction starting_direction;
	char		*floor;
	char		*ceiling;
	char ** grid;
	
}	t_map;

typedef struct	s_image
{
	t_sprite	north;
	t_sprite	south;
	t_sprite	west;
	t_sprite	east;
}	t_image;

typedef struct	s_game
{
	t_image		img;
	void		*mlx;
	void		*win;

}	t_game;


//--------------------------PARSING-----------------------
bool	check_map_extension(char *filename);
bool	check_if_map_is_valid(char *filename);


#endif
