/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   structs.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kelle <kelle@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/02 00:32:42 by kelle             #+#    #+#             */
/*   Updated: 2026/06/23 04:54:17 by kelle            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef STRUCTS_H
# define STRUCTS_H

typedef struct s_fp
{
	float	x;
	float	y;
}	t_fp;

typedef struct s_ip
{
	int	x;
	int	y;
}	t_ip;

typedef struct s_dp
{
	double	x;
	double	y;
}	t_dp;

typedef struct s_glyph_style
{
	int	scale;
	int	color;
}	t_glyph_style;

typedef struct s_fov_ray
{
	t_dp	left;
	t_dp	right;
	int		idx;
	int		total;
}	t_fov_ray;

typedef struct s_img
{
	void	*img;
	char	*addr;
	int		bits_per_pixel;
	int		line_length;
	int		endian;
}	t_img;

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
	char	**grid;
	t_rgb	*floor;
	t_rgb	*ceiling;
	int		start_x;
	int		start_y;
}	t_map;

typedef struct s_texture
{
	void	*path;
	t_img	img;
}	t_texture;

typedef struct s_sprites
{
	t_texture	north;
	t_texture	south;
	t_texture	west;
	t_texture	east;
	t_texture	white_square;
	t_texture	black_square;
	t_texture	test;
	t_texture	test1;
	t_texture	test2;
	t_texture	test3;
	t_texture	test4;
	t_texture	test5;
	t_texture	test6;
	t_texture	test7;
	t_texture	test8;
	t_texture	test9;
	t_texture	test10;
}	t_sprites;

typedef struct s_player
{
	double		player_x;
	double		player_y;
	double		dir_x;
	double		dir_y;
	double		plane_x;
	double		plane_y;
	bool		up;
	bool		down;
	bool		left;
	bool		right;
	bool		rotate_l;
	bool		rotate_r;
	bool		mouse_flag;
	t_direction	starting_direction;
}	t_player;

typedef enum e_door_state
{
	DOOR_CLOSED,
	DOOR_OPENING,
	DOOR_OPEN,
	DOOR_CLOSING,
}	t_door_state;

typedef struct s_door
{
	int				x;
	int				y;
	t_door_state	state;
	int				frame;
}	t_door;

typedef struct s_game
{
	t_player	player;
	t_door		*door;
	int			doors;
	t_sprites	sprites;
	t_img		buffer;
	void		*mlx;
	void		*win;
	int			win_w;
	int			win_h;
	t_map		*map;
	bool		debug;
	bool		minimap;
	int			minimap_tile_size;
	double		minimap_radius_tiles;
	double		movement_speed;
	double		rotation_speed;
	double		fov;
}	t_game;

typedef struct s_elem
{
	char	*id;
	char	**target;
}	t_elem;

typedef struct t_raycasting
{
	double		camera_x;
	double		ray_dir_x;
	double		ray_dir_y;
	double		side_dist_x;
	double		side_dist_y;
	double		delta_dist_x;
	double		delta_dist_y;
	double		perp_wall_dist;
	int			step_x;
	int			step_y;
	int			map_x;
	int			map_y;
	int			draw_end;
	int			draw_start;
	int			side;
	int			line_height;
	bool		hit;
	char		tile;
	int			tex_x;
	t_texture	texture;
}	t_ray;

#endif