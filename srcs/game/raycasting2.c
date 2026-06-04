#include "cub3d.h"

static void	load_texture(t_game *game, t_ray *ray)
{
	if (ray->side == 0)
	{
		if (ray->rayDirX > 0)
			ray->texture = game->sprites.east;
		else
			ray->texture = game->sprites.west;
	}
	else
	{
		if (ray->rayDirY > 0)
			ray->texture = game->sprites.south;
		else
			ray->texture = game->sprites.north;
	}
}

static void	find_wall_hit(t_game *game, t_ray *ray)
{
	double	wall_x;

	if (ray->side == 0)
		wall_x = game->player.player_y + ray->perpWallDist * ray->rayDirY;
	else
		wall_x = game->player.player_x + ray->perpWallDist * ray->rayDirX;
	wall_x -= floor(wall_x);
	ray->texX = (int)(wall_x * PX);
	if (ray->side == 0 && ray->rayDirX > 0)
		ray->texX = PX - ray->texX - 1;
	if (ray->side == 1 && ray->rayDirY < 0)
		ray->texX = PX - ray->texX - 1;
}

static unsigned int	get_texture_pixel(t_img *texture, int x, int y)
{
	int		offset;

	offset = y * texture->line_length + x * (texture->bits_per_pixel / 8);
	return (*(unsigned int *)(texture->addr + offset));
}

void	calculate_wall(t_game *game, t_ray *ray)
{
	if (ray->side == 0)
		ray->perpWallDist = ray->sideDistX - ray->deltaDistX;
	else
		ray->perpWallDist = ray->sideDistY - ray->deltaDistY;
	if (ray->perpWallDist == 0)
		ray->perpWallDist = 0.0001;
	ray->lineHeight = (int)(game->win_h / ray->perpWallDist);
	ray->DrawStart = -ray->lineHeight / 2 + game->win_h / 2;
	if (ray->DrawStart < 0)
		ray->DrawStart = 0;
	ray->DrawEnd = ray->lineHeight / 2 + game->win_h / 2;
	if (ray->DrawEnd >= game->win_h)
		ray->DrawEnd = game->win_h - 1;
}

void	draw_textured_line(t_game *game, t_ray *ray, int x)
{
	double			step;
	double			tex_pos;
	int				tex_y;
	int				y;
	unsigned int	color;

	step = (double)PX / ray->lineHeight;
	tex_pos = (ray->DrawStart - game->win_h / 2 + ray->lineHeight / 2) * step;
	y = ray->DrawStart;
	load_texture(game, ray);
	find_wall_hit(game, ray);
	while (y < ray->DrawEnd)
	{
		tex_y = (int)tex_pos & (PX - 1);
		tex_pos += step;
		if (ray->texX < 0 || ray->texX >= PX || tex_y < 0 || tex_y >= PX)
		{
			y++;
			continue ;
		}
		color = color_gradient(ray,
				get_texture_pixel(&ray->texture.img, ray->texX, tex_y));
		my_pixel_put(game, x, y, color);
		y++;
	}
}
