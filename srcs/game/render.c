/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   render.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kelle <kelle@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/23 02:33:09 by kelle             #+#    #+#             */
/*   Updated: 2026/06/02 18:51:48 by kelle            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

void	my_pixel_put(t_game *game, int x, int y, int color)
{
	char	*dst;
	int		offset;

	if (x < 0 || y < 0 )
		return;
	if (x >= game->win_w || y >= game->win_h)
		return ;
	offset = (y * game->buffer.line_length + x * (game->buffer.bits_per_pixel / 8));
	dst = game->buffer.addr + offset;
	*(unsigned int *)dst = color;
}
static int	create_trgb(int t, int r, int g, int b)
{
	return (t << 24 | r << 16 | g << 8 | b);
}

static void	paint_plain_background(t_game *game, int color)
{
	int	y;
	int	x;

	y = 0;
	while (y < game->win_h)
	{
		x = 0;
		while (x < game->win_w)
				my_pixel_put(game, x++, y, color);
		y++;
	}
}

static void	paint_background(t_game *game)
{
	int	y;
	int	x;
	int c;
	int f;

	y = 0;
	c = create_trgb(0, game->map->ceiling->r, game->map->ceiling->g,
		game->map->ceiling->b);
	f = create_trgb(0, game->map->floor->r, game->map->floor->g,
		game->map->floor->b);
	while (y < game->win_h)
	{
		x = 0;
		while (x < game->win_w)
		{
			if (y < game->win_h / 2)
				my_pixel_put(game, x, y, c);
			else
				my_pixel_put(game, x, y, f);
			x++;
		}
		y++;
	}
}

int	render(t_game *game)
{
	if (game->debug)
	{
		paint_plain_background(game, GREY);
		render_elements(game);
		move_player(game, &game->player);
		draw_texture(game, game->sprites.black_square.img, game->player.player_x * PX,
		game->player.player_y * PX, 3);
		print_fov(game, &game->player);
		if (game->minimap)
			render_minimap(game);
		mlx_put_image_to_window(game->mlx, game->win, game->buffer.img, 0, 0);
		display_speeds(game);
	}
	else
	{
		paint_background(game);
		move_player(game, &game->player);
		raycasting(game);
		if (game->minimap)
			render_minimap(game);
		mlx_put_image_to_window(game->mlx, game->win, game->buffer.img, 0, 0);
		display_speeds(game);
	}
	return (0);
}
