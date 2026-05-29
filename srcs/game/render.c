/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   render.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kelle <kelle@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/23 02:33:09 by kelle             #+#    #+#             */
/*   Updated: 2026/05/23 02:35:55 by kelle            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

void	my_pixel_put(t_img *img, int x, int y, int color)
{
	char	*dst;
	int		offset;

	offset = (y * img->line_length + x * (img->bits_per_pixel / 8));
	dst = img->addr + offset;
	*(unsigned int *)dst = color;
}

void	draw_texture(t_img *img, t_img sprite, float x, float y, int size)
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
			my_pixel_put(img, tex_x + x, tex_y + y, color);
			tex_x++;
		}
		tex_y++;
	}
}

static void	render_wall(t_game *game, t_img *img)
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
				draw_texture(img, game->sprites.east.img, x * PX, y * PX, PX);
			x++;
		}
		y++;
	}
}

int	create_trgb(int t, int r, int g, int b)
{
	return (t << 24 | r << 16 | g << 8 | b);
}

static void	paint_background(t_game *game, t_img *img)
{
	int	y;
	int	x;
	int c;
	int f;

	y = 0;
	c = create_trgb(0, game->map->ceiling->r, game->map->ceiling->g, game->map->ceiling->b);
	f = create_trgb(0, game->map->floor->r, game->map->floor->g, game->map->floor->b);
	while (y < game->win_h)
	{
		x = 0;
		while (x < game->win_w)
		{
			if (y < game->win_h / 2)
				my_pixel_put(img, x, y, c);
			else
				my_pixel_put(img, x, y, f);
			x++;
		}
		y++;
	}
}
static void	paint_plain_background(t_game *game, t_img *img, int color)
{
	int	y;
	int	x;

	y = 0;
	while (y < game->win_h)
	{
		x = 0;
		while (x < game->win_w)
				my_pixel_put(img, x++, y, color);
		y++;
	}
}

void print_rays(t_game *game, t_player *player, double rayDir_x, double rayDir_y)
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
		if (game->map->grid[(int)(world_y)][(int)(world_x)] == '1')
			break;
		my_pixel_put(&game->buffer.img, screen_x, screen_y, RED);
		j += 0.01;
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

// void ns()
// {
// 		if (rayDirX == 0)
// 			deltaDistX = 1e30;
// 		else 
// 			deltaDistX = ft_abs(1.0 / rayDirX);
// 		if (rayDirY == 0)
// 			deltaDistY = 1e30;
// 		else 
// 			deltaDistY = ft_abs(1.0 / rayDirY);
// 		if (rayDirX < 0)
// 		{
// 			stepX = -1;
// 			sideDistX = (player->player_x  - map_x) * deltaDistX;
// 		}
// 		else
// 		{
// 			stepX = 1;
// 			sideDistX = (map_x + 1.0 - player->player_x) * deltaDistX;
// 		}
// 		if (rayDirY < 0)
// 		{
// 			stepY = -1;
// 			sideDistY = (player->player_y  - map_y) * deltaDistY;
// 		}
// 		else
// 			stepY = 1;
// 			sideDistY = (map_y + 1.0 - player->player_y) * deltaDistY;
// }



int	render(t_game *game)
{
	bool debug;

	debug = false;
	if (debug)
	{
		paint_plain_background(game, &game->buffer.img, GREY);
		render_wall(game, &game->buffer.img);
		move_player(game, &game->player);
		draw_texture(&game->buffer.img, game->sprites.south.img, game->player.player_x * PX,
		game->player.player_y * PX, 3);
		print_fov(game, &game->player);
		mlx_put_image_to_window(game->mlx, game->win, game->buffer.img, 0, 0);
	}
	else
	{
		paint_background(game, &game->buffer.img);
		move_player(game, &game->player);
		raycasting(game);
		mlx_put_image_to_window(game->mlx, game->win, game->buffer.img, 0, 0);
	}
	return (0);
}
