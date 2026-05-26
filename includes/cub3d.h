/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cub3d.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kelle <kelle@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/16 01:10:41 by kelle             #+#    #+#             */
/*   Updated: 2026/05/15 18:52:48 by kelle            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CUB3D_H
#define CUB3D_H

#include "../libs/Libft/includes/libft.h"
#include <X11/X.h>
#include <X11/keysym.h>
#include <mlx.h>
#include <math.h>
#include <stdbool.h>
#include <time.h>
#include <errno.h>

#define PX 64
#define RED 0xFFFF0000
#define GREY 0x00666666
// #define WIDTH 
// #define HEIGHT

#define ALLOC_ERR "Memory allocation failed"
#define RGB_DIGIT_ERR "RGB must only have digits"
#define RGB_RANGE_ERR "RGB values must be between 0 to 255 range"
#define MLX "MLX initialization failed"
#define MLX_WIN "Window creation failed"
#define MLX_IMG "Image initialization failed"
#define MLX_ADDR "Failed to get image adress"

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
} t_game;

typedef struct s_elem
{
	char *id;
	char **target;
} t_elem;

//--------------------------PARSING-----------------------
bool check_extension(char *filename, char *extension);
bool check_if_map_is_valid(char *filename, t_game *game);
int get_map_details(t_game *game, char *filename);
int filter_rgb(char **arr, t_game *game);

// ver se realmente tem de ficar aqui no header ou se e so static
bool check_maze(t_game *game);
bool store_maze(t_game *game, char *full_line);
bool flood_fill_prep(t_map *map);
void get_starting_info(t_game *game);
char *get_validated_full_line(char *full_line);

//--------------------------GAME-----------------------
int render(t_game *game);
void draw_texture(t_img *img, t_img sprite, float x, float y, int size);
void render_player(t_game *game, t_img *img);
int player_moving(int keycode, void *param);
int player_idle(int keycode, void *param);
void my_pixel_put(t_img *img, int x, int y, int color);
void initialize_player(t_game *game, t_player *player);
void move_player(t_player *player);

//--------------------------UTILS-----------------------
void clear_matriz(char **matriz);
void print_error(char *msg);
void clear_game(t_game *game);
size_t array_len(char **array);
int ft_strcmp(char *s1, char *s2);
bool check_full_line(char *full_line);
char *ft_strjoin_and_free(char *s1, char *s2);
bool check_empty_line_on_map(char *full_line);
bool check_extension(char *filename, char *extension);
void remove_new_line_in_array(char **arr);
void remove_new_line(char *str);
int finish_game(void *param);
double ft_abs(double num);

#endif
