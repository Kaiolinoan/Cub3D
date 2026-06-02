#include "cub3d.h"

static void load_texture(t_game *game, t_ray *ray)
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

static void find_wall_hit(t_game *game, t_ray *ray)
{
	double wallX;

	if (ray->side == 0)
	    wallX = game->player.player_y + ray->perpWallDist * ray->rayDirY;
	else
	    wallX = game->player.player_x + ray->perpWallDist * ray->rayDirX;
	wallX -= floor(wallX);
	ray->texX = (int)(wallX * PX);
	if (ray->side == 0 && ray->rayDirX > 0)
    	ray->texX = PX - ray->texX - 1;
	if (ray->side == 1 && ray->rayDirY < 0)
    	ray->texX = PX - ray->texX - 1;
}

static unsigned int get_texture_pixel(t_img *texture, int x, int y)
{
    int offset;

    offset = y * texture->line_length + x * (texture->bits_per_pixel / 8);
    return (*(unsigned int *)(texture->addr + offset));
}

void calculate_wall(t_game *game, t_ray *ray)
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
}

int color_gradient(t_ray *ray, int color)
{
	double factor;

	factor = 1.0 - (ray->perpWallDist / 12.0);

	// if (ray->perpWallDist < 5.0)
    // 	factor = 1.0;
	// else
	// 	factor = 1.0 / (1.0 + (ray->perpWallDist - 5.0) * 0.1);
	if (factor < 0)
	    factor = 0;
	char r = ((color >> 16) & 0xFF) * factor;
	char g = ((color >> 8) & 0xFF) * factor;
	char b = (color & 0xFF) * factor;
	color = (r << 16) | (g << 8) | b;
	if (ray->side == 1)
    	color = (color >> 1) & 0x7F7F7F;
	return (color);
}

int color_gradient2(t_ray *ray, int color)
{
    double factor;

    // 1. Exponential drop-off: very bright up close, drops fast
    // Adjust the 1.5 to change how fast the brightness decays.
    factor = 1.5 / (1.0 + ray->perpWallDist * 0.3);

    // Caps the brightness so it doesn't overexpose completely white
    if (factor > 1.2) 
        factor = 1.2; 
    if (factor < 0.0)
        factor = 0.0;

    // 2. Extract channels using unsigned ints to prevent sign-extension bugs
    unsigned int r = (unsigned int)(((color >> 16) & 0xFF) * factor);
    unsigned int g = (unsigned int)(((color >> 8)  & 0xFF) * factor);
    unsigned int b = (unsigned int)((color & 0xFF)         * factor);

    // Clamp values to 255 so colors don't bleed into other channels
    if (r > 255) r = 255;
    if (g > 255) g = 255;
    if (b > 255) b = 255;

    // Reconstruct the color
    color = (r << 16) | (g << 8) | b;

    // 3. Keep your side-shading for 3D depth (makes Y-axis walls 50% darker)
    if (ray->side == 1)
        color = (color >> 1) & 0x7F7F7F;

    return (color);
}

int color_gradient3(t_ray *ray, int color)
{
    double factor;
    double max_dist;

    max_dist = 12.0;

    // 1. If it's past your max distance, it's pitch black immediately
    if (ray->perpWallDist >= max_dist)
        return (0x000000);

    // 2. Combine them: 
    // (1.5 / (1.0 + dist * 0.3)) gives the aggressive brightness up close.
    // (1.0 - (dist / max_dist)) forces it to scale down to 0 exactly at max_dist.
    factor = (1.5 / (1.0 + ray->perpWallDist * 0.3)) * (1.0 - (ray->perpWallDist / max_dist));

    // Cap the overexposure brightness so it doesn't break color logic
    if (factor > 1.3) 
        factor = 1.3;
    if (factor < 0.0)
        factor = 0.0;

    // 3. Extract channels safely using unsigned ints
    unsigned int r = (unsigned int)(((color >> 16) & 0xFF) * factor);
    unsigned int g = (unsigned int)(((color >> 8)  & 0xFF) * factor);
    unsigned int b = (unsigned int)((color & 0xFF)         * factor);

    // Clamp to prevent channel bleeding
    if (r > 255) r = 255;
    if (g > 255) g = 255;
    if (b > 255) b = 255;

    color = (r << 16) | (g << 8) | b;

    // 4. Apply your side-shading for the Y-axis walls (creates 3D depth)
    if (ray->side == 1)
        color = (color >> 1) & 0x7F7F7F;

    return (color);
}
void draw_textured_line(t_game *game, t_ray *ray, int x)
{
	double step;
	double texPos;
	unsigned int color;

	step = (double)PX / ray->lineHeight;
	texPos = (ray->DrawStart - game->win_h / 2 + ray->lineHeight / 2) * step;
	int y = ray->DrawStart;
	load_texture(game, ray);
	find_wall_hit(game, ray);
	while (y < ray->DrawEnd)
	{
	    int texY;
	    texY = (int)texPos & (PX - 1);
	    texPos += step;
		if (ray->texX < 0 || ray->texX >= PX || texY < 0 || texY >= PX)
		{
		    y++;
		    continue;
		}
	    color = color_gradient3(ray, get_texture_pixel(&ray->texture.img, ray->texX, texY));
	    my_pixel_put(game, x, y, color);
	    y++;
	}
}
