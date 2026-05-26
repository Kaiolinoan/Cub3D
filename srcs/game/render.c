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

static void	paint_background(t_game *game, t_img *img)
{
	int	y;
	int	x;

	y = 0;
	while (y < game->win_h)
	{
		x = 0;
		while (x < game->win_w)
			my_pixel_put(img, x++, y, GREY);
		y++;
	}
}

void print_rays(t_player *player, t_img *img)
{
	int	px;
	int	py;
	px = player->player_x * PX;
	py = player->player_y * PX;
	double ray_x = player->dir_x - player->plane_x;
	double ray_y = player->dir_y - player->plane_y;
	double ray_x2 = player->dir_x + player->plane_x;
	double ray_y2 = player->dir_y + player->plane_y;
	int j = 0;
	while (j < 100)
	{
		my_pixel_put(img, px + ray_x * j,
			py + ray_y * j, RED);
		j++;
	}
    int i = 0;
	while (i < 100)
	{
		my_pixel_put(img, px + ray_x2 * i,
			py + ray_y2 * i, RED);
		i++;
	}
		// i = 0;
	// while (i < 200)
	// {
	// 	my_pixel_put(img, px + player->dir_x * i,
	// 		py + player->dir_y * i, RED);
	// 	i++;
	// }=p
}

int raycasting (t_game *game, t_player *player, t_img *img) 
{
	double cameraX;
	double rayDirX;
	double rayDirY;
	double sideDistX;
	double sideDistY;
	double deltaDistX;
	double deltaDistY;
	double perpWallDist;
	int stepX;
	int stepY;
	int map_x;
	int map_y;
	int x;
	bool hit;
	int side;
	int lineHeight;
	int DrawStart;
	int DrawEnd;
	x = 0;
	while (x < game->win_w)
	{
		cameraX = 2 * x / (double)game->win_w - 1;
		rayDirX = player->dir_x + player->plane_x * cameraX;
		rayDirY = player->dir_y + player->plane_y * cameraX;
		map_x = (int) player->player_x;
		map_y = (int) player->player_y;
		hit = false;
		if (rayDirX == 0)
			deltaDistX = 1e30;
		else 
			deltaDistX = ft_abs(1.0 / rayDirX);
		if (rayDirY == 0)
			deltaDistY = 1e30;
		else 
			deltaDistY = ft_abs(1.0 / rayDirY);
		if (rayDirX < 0)
		{
			stepX = -1;
			sideDistX = (player->player_x  - map_x) * deltaDistX;
		}
		else
		{
			stepX = 1;
			sideDistX = (map_x + 1.0 - player->player_x) * deltaDistX;
		}
		if (rayDirY < 0)
		{
			stepY = -1;
			sideDistY = (player->player_y  - map_y) * deltaDistY;
		}
		else
		{
			stepY = 1;
			sideDistY = (map_y + 1.0 - player->player_y) * deltaDistY;
		}
		while (!hit)
		{
			if (sideDistX < sideDistY)
			{
				sideDistX += deltaDistX;
				map_x += stepX;
				side = 0;
			}
			else
			{
				sideDistY += deltaDistY;
				map_y += stepY;
				side = 1;
			}
			if (game->map->grid[map_y][map_x] == '1')
				hit = 1;
		}
		if (side == 0)
			perpWallDist = sideDistX - deltaDistX;
		else
			perpWallDist = sideDistY - deltaDistY;
		if (perpWallDist == 0)
    		perpWallDist = 0.0001;
		lineHeight = (int) (game->win_h / perpWallDist);
		DrawStart = -lineHeight / 2 + game->win_h / 2;
		if (DrawStart < 0)
			DrawStart = 0;
		DrawEnd = lineHeight / 2 + game->win_h / 2;
		if (DrawEnd >= game->win_h)
			DrawEnd = game->win_h - 1;
		int y = DrawStart;
		while (y < DrawEnd)
		{
		    my_pixel_put(img, x, y, RED);
		    y++;
		}
		x++;
	}
	return (0);
}

int	render(t_game *game)
{
	bool debug;

	debug = false;
	if (debug)
	{
		paint_background(game, &game->buffer.img);
		render_wall(game, &game->buffer.img);
		move_player(&game->player);
		draw_texture(&game->buffer.img, game->sprites.south.img, game->player.player_x * PX,
		game->player.player_y * PX, 20);
		print_rays(&game->player, &game->buffer.img);
		mlx_put_image_to_window(game->mlx, game->win, game->buffer.img, 0, 0);
	}
	else
	{
		paint_background(game, &game->buffer.img);
		move_player(&game->player);
		raycasting(game, &game->player, &game->buffer.img);
		mlx_put_image_to_window(game->mlx, game->win, game->buffer.img, 0, 0);
	}
	return (0);
}
