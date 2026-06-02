#ifndef STRUCTS_H
#define STRUCTS_H

typedef struct s_img
{
	void *img;
	char *addr;
	int bits_per_pixel;
	int line_length;
	int endian;
} t_img;

typedef enum e_direction
{
	NORTH,
	SOUTH,
	WEST,
	EAST,
} t_direction;

typedef struct s_rgb
{
	unsigned char r;
	unsigned char g;
	unsigned char b;
} t_rgb;

typedef struct s_map
{
	char **grid;
	t_rgb *floor;
	t_rgb *ceiling;
	int start_x;
	int start_y;
} t_map;

typedef struct s_texture
{
	void *path;
	t_img img;
}	t_texture;

typedef struct s_sprites
{
	t_texture north;
	t_texture south;
	t_texture west;
	t_texture east;
	t_texture white_square;
	t_texture black_square;
	t_texture test;
	t_texture test1;
	t_texture test2;
	t_texture test3;
	t_texture test4;
	t_texture test5;
	t_texture test6;
	t_texture test7;
	t_texture test8;
	t_texture test9;
	t_texture test10;
} t_sprites;

typedef struct s_player
{
	double player_x;
	double player_y;
	double dir_x;
	double dir_y;
	double plane_x;
	double plane_y;

	bool up;
	bool down;
	bool left;
	bool right;

	bool rotate_l;
	bool rotate_r;
	t_direction starting_direction;
}	t_player;

typedef struct s_game
{
	t_player	player;
	t_sprites	sprites;
	t_img 		buffer;
	void		*mlx;
	void		*win;
	int			win_w;
	int			win_h;
	t_map		*map;
	bool		debug;
} t_game;

typedef struct s_elem
{
	char *id;
	char **target;
} t_elem;

typedef struct t_raycasting
{
	double	cameraX;
	double	rayDirX;
	double	rayDirY;
	double	sideDistX;
	double	sideDistY;
	double	deltaDistX;
	double	deltaDistY;
	double	perpWallDist;
	int		stepX;
	int		stepY;
	int		map_x;
	int		map_y;
	int		DrawEnd;
	int		DrawStart;
	int		side;
	int		lineHeight;
	bool	hit;
	int		texX;
	t_texture texture;
} t_ray;

#endif