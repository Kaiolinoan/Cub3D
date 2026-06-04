#include "cub3d.h"
static void set_image_path(t_texture *tex, char *path)
{
    tex->path = ft_strdup(path);
}   
static void	file_to_image(void *mlx, t_texture *dir)
{
	int	size;

	size = PX;
	dir->img.img = mlx_xpm_file_to_image(mlx, dir->path, &size, &size);
}

static void	get_dir_img_address(t_texture *dir)
{
	dir->img.addr = mlx_get_data_addr(dir->img.img, &dir->img.bits_per_pixel,
			&dir->img.line_length, &dir->img.endian);
}

bool	initialize_images(t_game *game, t_sprites *sprites)
{
	game->buffer.img = mlx_new_image(game->mlx, game->win_w, game->win_h);
    set_image_path(&sprites->black_square, "assets/black_square-64.xpm");
    set_image_path(&sprites->white_square, "assets/square-64.xpm");
    set_image_path(&sprites->test1, "assets/test1.xpm");
    set_image_path(&sprites->test2, "assets/grey_yellow.xpm");
    set_image_path(&sprites->test3, "assets/grey_pink.xpm");
    set_image_path(&sprites->test4, "assets/gemini.xpm");
    set_image_path(&sprites->test5, "assets/test.xpm");
    set_image_path(&sprites->test6, "assets/test2.xpm");
    set_image_path(&sprites->test7, "assets/bright.xpm");
	file_to_image(game->mlx, &sprites->east);
	file_to_image(game->mlx, &sprites->west);
	file_to_image(game->mlx, &sprites->north);
	file_to_image(game->mlx, &sprites->south);
	file_to_image(game->mlx, &sprites->white_square);
	file_to_image(game->mlx, &sprites->black_square);
	// file_to_image(game->mlx, &sprites->test);
	file_to_image(game->mlx, &sprites->test1);
	file_to_image(game->mlx, &sprites->test2);
	file_to_image(game->mlx, &sprites->test3);
	file_to_image(game->mlx, &sprites->test4);
	file_to_image(game->mlx, &sprites->test5);
	file_to_image(game->mlx, &sprites->test6);
	file_to_image(game->mlx, &sprites->test7);
	// file_to_image(game->mlx, &sprites->test8);
	// file_to_image(game->mlx, &sprites->test9);
	// file_to_image(game->mlx, &sprites->test10);
	if (!game->buffer.img || !sprites->east.img.img || !sprites->west.img.img
        || !sprites->north.img.img || !sprites->south.img.img
        || !sprites->white_square.img.img || !sprites->black_square.img.img)
		return (print_error(MLX_IMG), 0);
	game->buffer.addr = mlx_get_data_addr(game->buffer.img,
			&game->buffer.bits_per_pixel, &game->buffer.line_length,
			&game->buffer.endian);
	get_dir_img_address(&sprites->east);
	get_dir_img_address(&sprites->west);
	get_dir_img_address(&sprites->north);
	get_dir_img_address(&sprites->south);
	get_dir_img_address(&sprites->white_square);
	get_dir_img_address(&sprites->black_square);
	// get_dir_img_address(&sprites->test);
	get_dir_img_address(&sprites->test1);
	get_dir_img_address(&sprites->test2);
	get_dir_img_address(&sprites->test3);
	get_dir_img_address(&sprites->test4);
	get_dir_img_address(&sprites->test5);
	get_dir_img_address(&sprites->test6);
	get_dir_img_address(&sprites->test7);
	// get_dir_img_address(&sprites->test8);
	// get_dir_img_address(&sprites->test9);
	// get_dir_img_address(&sprites->test10);
	if (!game->buffer.addr || !sprites->east.img.addr || !sprites->west.img.addr 
        || !sprites->north.img.addr || !sprites->south.img.addr 
        || !sprites->white_square.img.addr || !sprites->black_square.img.addr)
			return (print_error(MLX_ADDR), 0);
	return (1);
}

t_game	*initialize_game(char *filename)
{
	t_game	*game;

	game = ft_calloc(1, sizeof(t_game));
	if (!game)
		return (print_error(ALLOC_ERR), NULL);
	game->map = ft_calloc(1, sizeof(t_map));
	if (!game->map)
		return (clear_game(game), print_error(ALLOC_ERR), NULL);
	if (!get_map_details(game, filename))
		return (clear_game(game), NULL);
	game->minimap = true;
	game->minimap_tile_size = 20;
	game->minimap_radius_tiles = 7.0;
	game->movement_speed = 0.07;
	game->rotation_speed = 0.03;
	game->fov = 0.66;
	return (game);
}
