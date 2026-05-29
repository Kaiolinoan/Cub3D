#include "cub3d.h"

void cast_ray(t_game *game, t_ray *ray)
{
    while (!ray->hit)
	{
		if (ray->sideDistX < ray->sideDistY)
		{
			ray->sideDistX += ray->deltaDistX;
			ray->map_x += ray->stepX;
			ray->side = 0;
		}
		else
		{
			ray->sideDistY += ray->deltaDistY;
			ray->map_y += ray->stepY;
			ray->side = 1;
		}
		if (game->map->grid[ray->map_y][ray->map_x] == '1')
			ray->hit = 1;
	}
}

void calculate_and_draw_line(t_game *game, t_ray *ray, int x)
{
	if (ray->side == 0)
		ray->perpWallDist = ray->sideDistX - ray->deltaDistX;
	else
		ray->perpWallDist = ray->sideDistY - ray->deltaDistY;
	if (ray->perpWallDist == 0)
    	ray->perpWallDist = 0.0001;
	ray->lineHeight = (int) (game->win_h / ray->perpWallDist);
	ray->DrawStart = -ray->lineHeight / 2 + game->win_h / 2;
	if (ray->DrawStart < 0)
		ray->DrawStart = 0;
	ray->DrawEnd = ray->lineHeight / 2 + game->win_h / 2;
	if (ray->DrawEnd >= game->win_h)
		ray->DrawEnd = game->win_h - 1;
	int y = ray->DrawStart;
	while (y < ray->DrawEnd)
	{
	    my_pixel_put(&game->buffer.img, x, y, GREY);
	    y++;
	}
}

void check_raydir_x(t_player *player, t_ray *ray)
{
    if (ray->rayDirX == 0)
		ray->deltaDistX = 1e30;
	else 
		ray->deltaDistX = ft_abs(1.0 / ray->rayDirX);
    if (ray->rayDirX < 0)
	{
		ray->stepX = -1;
		ray->sideDistX = (player->player_x  - ray->map_x) * ray->deltaDistX;
	}
	else
	{
		ray->stepX = 1;
		ray->sideDistX = (ray->map_x + 1.0 - player->player_x) * ray->deltaDistX;
	}
}
void check_raydir_y(t_player *player, t_ray *ray)
{
	if (ray->rayDirY == 0)
		ray->deltaDistY = 1e30;
	else 
		ray->deltaDistY = ft_abs(1.0 / ray->rayDirY);	
	if (ray->rayDirY < 0)
	{
		ray->stepY = -1;
		ray->sideDistY = (player->player_y  - ray->map_y) * ray->deltaDistY;
	}
	else
	{
		ray->stepY = 1;
		ray->sideDistY = (ray->map_y + 1.0 - player->player_y) * ray->deltaDistY;
	}
}

void init_ray(t_game *game, t_player *player, t_ray *ray, int x)
{
    ray->cameraX = 2 * x / (double)game->win_w - 1;
	ray->rayDirX = player->dir_x + player->plane_x * ray->cameraX;
	ray->rayDirY = player->dir_y + player->plane_y * ray->cameraX;
	ray->map_x = (int) player->player_x;
	ray->map_y = (int) player->player_y;
	ray->hit = false;
    check_raydir_x(player, ray);
    check_raydir_y(player, ray);
}

int raycasting (t_game *game)
{
    t_ray ray;
    int x;

    x = 0;
    while (x < game->win_w)
	{
		init_ray(game, &game->player, &ray, x);
        cast_ray(game, &ray);
		calculate_and_draw_line(game, &ray, x);
        x++;
    }
	return (0);
}
