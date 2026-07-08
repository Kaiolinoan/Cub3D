/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cub3d.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kelle <kelle@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/16 01:10:41 by kelle             #+#    #+#             */
/*   Updated: 2026/07/07 20:19:48 by kelle            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CUB3D_H
# define CUB3D_H

# include "../libs/Libft/includes/libft.h"
# include <X11/X.h>
# include <X11/keysym.h>
# include <mlx.h>
# include <math.h>
# include <stdbool.h>
# include <time.h>
# include <errno.h>
# include <fcntl.h>
# include <signal.h>
# include <unistd.h>
# include "structs.h"

# define PX 64
# define RED 0xFFFF0000
# define GREY 0x00666666
# define GREEN 0X0000FF00
# define BLUE 0X000000FF
# define WIDTH 1920
# define HEIGHT 1024

# define ALLOC_ERR "Memory allocation failed"
# define RGB_DIGIT_ERR "RGB must only have digits"
# define RGB_RANGE_ERR "RGB values must be between 0 to 255 range"
# define MLX "MLX initialization failed"
# define MLX_WIN "Window creation failed"
# define MLX_IMG "Image initialization failed"
# define MLX_ADDR "Failed to get image adress"

// Minimap
# define MINIMAP_TILE_SIZE 20
# define MINIMAP_RADIUS_TILES 7.0
# define MINIMAP_MARGIN 20
# define MINIMAP_FLOOR_COLOR 0x00EDF4FC
# define MINIMAP_WALL_COLOR 0x00001F8C
# define MINIMAP_VOID_COLOR 0x00181818
# define MINIMAP_BORDER_COLOR 0x00000000
# define MINIMAP_PLAYER_COLOR 0x00FF4C4C
# define MINIMAP_DOOR_COLOR 0x008A4E36
# define MINIMAP_FOV_COLOR 0x0000E5FF

// Door
# define DOOR_RANGE 1.5
# define DOOR_ANIM_FRAMES 14

// Door Sounds
# define DOOR_OPEN_SOUND "assets/sounds/door_open.wav"
# define DOOR_CLOSE_SOUND "assets/sounds/door_close.wav"
//--------------------------PARSING-----------------------

//rgb
int				filter_rgb(char **arr, t_game *game);

//elements
int				get_map_details(t_game *game, char *filename);

//map
bool			check_if_map_is_valid(char *filename, t_game *game);
bool			store_maze(t_game *game, char *full_line);

// Door Parsing
bool			check_doors(t_game *game);

//info
void			get_starting_info(t_game *game);
char			*get_validated_full_line(char *full_line);

//flood fill
bool			flood_fill_prep(t_map *map);

//--------------------------GAME-----------------------

//=====================// Minimap //====================//

// Minimap Main
void			render_minimap(t_game *game);

// Minimap FOV
void			draw_minimap_fov(t_game *game, int center_x, int center_y);
void			draw_minimap_fov_line(t_game *game, t_ip start, t_dp end);

// Minimap FOV Utils
unsigned int	blend_color_with_white(unsigned int base_color,
					double white_opacity);
bool			minimap_in_circle(int x, int y, t_ip center, int radius_px);

// Minimap Player
void			draw_minimap_player(t_game *game, int center_x, int center_y);

// Minimap Utils
void			display_minimap(t_game *game);
int				minimap_map_height(t_map *map);
unsigned int	rgb_to_color(t_rgb *rgb);
void			put_minimap_pixel(t_game *game, int x, int y,
					unsigned int color);
unsigned int	minimap_tile_color(t_game *game, double world_x,
					double world_y);

//debug
void			render_player(t_game *game, t_img *img);
void			render_elements(t_game *game);
void			print_fov(t_game *game, t_player *player);
void			draw_texture(t_game *game, t_img sprite, t_fp p, int size);

//initialization
bool			initialize_images(t_game *game, t_sprites *sprites);
t_game			*initialize_game(char *filename);

//mlx
void			mlx_main(t_game *game);
int				finish_game(void *param);

//doors
void			update_doors(t_game *game);
t_door			*get_door_at(t_game *game, int x, int y);
bool			is_door_blocking(t_game *game, int x, int y);

// door sound
void			init_door_audio(void);
void			play_sound(char *path);

// door frames
bool			load_door_frames(t_game *game, t_sprites *sprites);

//movement
void			rotate(t_player *player, double rot);
void			move_player(t_game *game, t_player *player);

//player
int				player_moving(int keycode, void *param);
int				player_idle(int keycode, void *param);
void			initialize_player(t_game *game, t_player *player);

//render
int				render(t_game *game);
void			my_pixel_put(t_game *game, int x, int y, int color);

//raycasting 
int				raycasting(t_game *game);
void			calculate_wall(t_game *game, t_ray *ray);
void			draw_textured_line(t_game *game, t_ray *ray, int x, int y);
void			cast_ray(t_game *game, t_ray *ray, char tile);
void			check_raydir_y(t_player *player, t_ray *ray);
void			check_raydir_x(t_player *player, t_ray *ray);
bool			ray_hits_door(t_game *game, t_ray *ray);
unsigned int	get_texture_pixel(t_img *texture, int x, int y);
void			find_wall_hit(t_game *game, t_ray *ray);
void			load_texture(t_game *game, t_ray *ray);

//color gradient
int				color_gradient(t_ray *ray, int color);

// keys handle
void			handle_speed_keys(int keycode, t_game *game);
void			handle_minimap_keys(int keycode, t_game *game);
void			handle_keys(int keycode, t_player *player);

//--------------------------UTILS-----------------------

//utils
void			print_error(char *msg);
size_t			array_len(char **array);
void			remove_new_line_in_array(char **arr);
void			remove_new_line(char *str);
double			ft_abs(double num);

//parsing
bool			check_full_line(char *full_line);
bool			check_empty_line_on_map(char *full_line);
char			*ft_strjoin_and_free(char *s1, char *s2);
int				ft_strcmp(char *s1, char *s2);
bool			check_extension(char *filename, char *extension);

//text_render
void			draw_string_scaled(t_game *game, char *str, t_ip pos,
					int scale);
void			display_speeds(t_game *game);

// door
t_door			*get_door_at(t_game *game, int x, int y);
char			get_char_at(char **grid, int y, int x);
bool			is_walkable(char c);
void			clear_door(t_game *game, size_t i);
void			clear_door_frames(void *mlx, t_texture *frames, int count);

//clean
void			clear_matriz(char **matriz);
void			clear_game(t_game *game);

//init_utils
bool			set_image_path(t_texture *tex, char *path);
bool			file_to_image(void *mlx, t_texture *dir);
bool			get_dir_img_address(t_texture *dir);

#endif
