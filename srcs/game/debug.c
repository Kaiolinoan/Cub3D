#include "cub3d.h"

void	draw_texture(t_game *game, t_img sprite, float x, float y, int size)
{
	int				tex_y;
	int				tex_x;
	int				offset;
	unsigned int	color;

	tex_y = 0;
	while (tex_y < size)
	{
		tex_x = 0;
		while (tex_x < size)
		{
			offset = (tex_y * sprite.line_length)
				+ (tex_x * sprite.bits_per_pixel / 8);
			color = *(unsigned int *)(sprite.addr + offset);
			my_pixel_put(game, tex_x + x, tex_y + y, color);
			tex_x++;
		}
		tex_y++;
	}
}

void	render_elements(t_game *game)
{
	char	**map;
	int		y;
	int		x;

	y = 0;
	map = game->map->grid;
	while (map[y])
	{
		x = 0;
		while (map[y][x])
		{
			if (map[y][x] == '1')
				draw_texture(game, game->sprites.white_square.img, x * PX, y * PX, PX);
			else if (map[y][x] == ' ')
				draw_texture(game, game->sprites.black_square.img, x * PX, y * PX, PX);
			x++;
		}
		y++;
	}
}

static void    print_rays(t_game *game, t_player *player, double rayDir_x, double rayDir_y)
{
	double j;
	double world_x; 
	double world_y;
	double screen_x;
	double screen_y;

	j = 0;
	while (j < game->win_w)
	{
		world_x = player->player_x + rayDir_x * j;
		world_y = player->player_y + rayDir_y * j;
		screen_x = world_x * PX;
		screen_y = world_y * PX;
		if (world_x < 0 || world_y < 0
			|| world_y >= array_len(game->map->grid)
			|| world_x >= ft_strlen(game->map->grid[(int)world_y]))
    		break;
		if (game->map->grid[(int)(world_y)][(int)(world_x)] == '1')
			break;
		my_pixel_put(game, screen_x, screen_y, RED);
		j += 0.05;
	}
}

void print_fov(t_game *game, t_player *player)
{
	int x;
	double cameraX;
	double rayDir_x;
	double rayDir_y;
	x = 0;
	while (x < game->win_w)
	{
		cameraX = 2 * x / (double)game->win_w - 1;
		rayDir_x = player->dir_x + player->plane_x * cameraX;
		rayDir_y = player->dir_y + player->plane_y * cameraX;
		print_rays(game, player, rayDir_x, rayDir_y);
		x++;
	}
}