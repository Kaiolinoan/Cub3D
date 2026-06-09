#include "cub3d.h"

static double   get_factor(t_ray *ray)
{
	double		max_dist;
	double		factor;

	max_dist = 12.0;
	if (ray->perp_wall_dist >= max_dist)
		return (0x000000);
	factor = (1.5 / (1.0 + ray->perp_wall_dist * 0.3))
		* (1.0 - (ray->perp_wall_dist / max_dist));
	if (factor > 1.3)
		factor = 1.3;
	if (factor < 0.0)
		factor = 0.0;
	return (factor);
}

int	color_gradient(t_ray *ray, int color)
{
	double			factor;
	unsigned int	r;
	unsigned int	g;
	unsigned int	b;

	factor = get_factor(ray);
	r = (unsigned int)(((color >> 16) & 0xFF) * factor);
	g = (unsigned int)(((color >> 8) & 0xFF) * factor);
	b = (unsigned int)((color & 0xFF) * factor);
	if (r > 255)
		r = 255;
	if (g > 255)
		g = 255;
	if (b > 255)
		b = 255;
	color = (r << 16) | (g << 8) | b;
	if (ray->side == 1)
		color = (color >> 1) & 0x7F7F7F;
	return (color);
}

// int color_gradient(t_ray *ray, int color)
// {
// 	double factor;

// 	factor = 1.0 - (ray->perp_wall_dist / 12.0);

// 	// if (ray->perp_wall_dist < 5.0)
//     // 	factor = 1.0;
// 	// else
// 	// 	factor = 1.0 / (1.0 + (ray->perp_wall_dist - 5.0) * 0.1);
// 	if (factor < 0)
// 	    factor = 0;
// 	char r = ((color >> 16) & 0xFF) * factor;
// 	char g = ((color >> 8) & 0xFF) * factor;
// 	char b = (color & 0xFF) * factor;
// 	color = (r << 16) | (g << 8) | b;
// 	if (ray->side == 1)
//     	color = (color >> 1) & 0x7F7F7F;
// 	return (color);
// }
