#include "cub3d.h"

void my_pixel_put(t_img *img, int x, int y, int color)
{
	char *dst;
	int offset;

	offset = (y * img->line_length + x * (img->bits_per_pixel / 8));
	dst = img->addr + offset;
	*(unsigned int *)dst = color;
}

void draw_texture(t_img *img, t_img sprite, float x, float y, int size)
{
	int tex_y;
	int tex_x;

	tex_y= 0;
	while (tex_y < size)
	{
		tex_x = 0;
		while (tex_x < size)
		{
			int offset = (tex_y * sprite.line_length) + (tex_x * sprite.bits_per_pixel / 8);
			unsigned int color = *(unsigned int *)(sprite.addr + offset);
			my_pixel_put(img, tex_x + x, tex_y + y, color);
			tex_x++;
		}
		tex_y++;
	}
}

static void render_wall(t_game *game, t_img *img)
{
	char **map;
	int y;
	int x;

	y = 0;
	map = game->map->grid;
	while (map[y])
	{
		x = 0;
		while (map[y][x])
		{
			if (map[y][x] == '1')
				draw_texture(img, game->sprites.east.img, x * PX , y * PX, PX);
			x++;
		}
		y++;
	}
}

static void paint_background(t_game *game, t_img *img)
{
	int y;
	int x;

	y = 0;
	while (y < game->win_h)
	{
		x = 0;
		while (x < game->win_w)
			my_pixel_put(img, x++, y, GREY);
		y++;
	}
}

int render(t_game *game)
{
	paint_background(game, &game->buffer.img);
	render_wall(game, &game->buffer.img);
	render_player(game, &game->buffer.img);
	mlx_put_image_to_window(game->mlx, game->win, game->buffer.img, 0, 0);
	return (0);
}