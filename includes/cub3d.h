/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cub3d.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kelle <kelle@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/16 01:10:41 by kelle             #+#    #+#             */
/*   Updated: 2026/06/02 19:08:34 by kelle            ###   ########.fr       */
/*   Updated: 2026/06/02 19:08:34 by kelle            ###   ########.fr       */
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

/* minimap */
# define MINIMAP_TILE_SIZE 20
# define MINIMAP_RADIUS_TILES 7.0
# define MINIMAP_MARGIN 20
# define MINIMAP_FLOOR_COLOR 0x00EDF4FC
# define MINIMAP_WALL_COLOR 0x00001F8C
# define MINIMAP_VOID_COLOR 0x00181818
# define MINIMAP_BORDER_COLOR 0x00000000
# define MINIMAP_PLAYER_COLOR 0x00FF4C4C
# define MINIMAP_FOV_COLOR 0x0000E5FF

//--------------------------PARSING-----------------------
bool	check_extension(char *filename, char *extension);
bool	check_if_map_is_valid(char *filename, t_game *game);
int		get_map_details(t_game *game, char *filename);
int		filter_rgb(char **arr, t_game *game);

// ver se realmente tem de ficar aqui no header ou se e so static
bool	check_maze(t_game *game);
bool	store_maze(t_game *game, char *full_line);
bool	flood_fill_prep(t_map *map);
void	get_starting_info(t_game *game);
char	*get_validated_full_line(char *full_line);

//--------------------------GAME-----------------------
int			render(t_game *game);
int			player_moving(int keycode, void *param);
int			raycasting (t_game *game);
int			player_idle(int keycode, void *param);
bool		initialize_images(t_game *game, t_sprites *sprites);
void		render_player(t_game *game, t_img *img);
void		draw_texture(t_game *game, t_img sprite, float x, float y, int size);
void		my_pixel_put(t_game *game, int x, int y, int color);
void		initialize_player(t_game *game, t_player *player);
void		move_player(t_game *game, t_player *player);
void		render_elements(t_game *game);
void 		print_fov(t_game *game, t_player *player);
void 		draw_textured_line(t_game *game, t_ray *ray, int x);
void 		calculate_wall(t_game *game, t_ray *ray);
void		mlx_main(t_game *game);
t_game		*initialize_game(char *filename);
void	    render_minimap(t_game *game);
void	    display_minimap(t_game *game);
void    	rotate(t_player *player, double rot);


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
void	remove_new_line_in_array(char **arr);
void	remove_new_line(char *str);
int		finish_game(void *param);
double	ft_abs(double num);
void	draw_string_scaled(t_game *game, char *str, int x, int y, int scale);
void	display_speeds(t_game *game);
#endif
