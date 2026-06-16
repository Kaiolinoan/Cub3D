/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kelle <kelle@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/10 19:12:36 by kelle             #+#    #+#             */
/*   Updated: 2026/06/10 19:12:37 by kelle            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

bool	set_image_path(t_texture *tex, char *path)
{
	tex->path = ft_strdup(path);
	if (!tex->path)
		return (print_error(ALLOC_ERR), false);
	return (true);
}

bool	file_to_image(void *mlx, t_texture *dir)
{
	int	size;

	size = PX;
	dir->img.img = mlx_xpm_file_to_image(mlx, dir->path, &size, &size);
	if (!dir->img.img)
		return (print_error(MLX_IMG), false);
	return (true);
}

bool	get_dir_img_address(t_texture *dir)
{
	dir->img.addr = mlx_get_data_addr(dir->img.img, &dir->img.bits_per_pixel,
			&dir->img.line_length, &dir->img.endian);
	if (!dir->img.addr)
		return (print_error(MLX_ADDR), false);
	return (true);
}