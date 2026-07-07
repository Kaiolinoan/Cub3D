/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   door_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/23 03:47:52 by kelle             #+#    #+#             */
/*   Updated: 2026/07/02 19:44:16 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

void	clear_door_frames(void *mlx, t_texture *frames, int count)
{
	int	i;

	i = 0;
	if (!frames)
		return ;
	while (i < count)
	{
		if (frames[i].img.img)
			mlx_destroy_image(mlx, frames[i].img.img);
		if (frames[i].path)
			free(frames[i].path);
		i++;
	}
	free(frames);
}

void	clear_door(t_game *game, size_t i)
{
	while (i < DOOR_ANIM_FRAMES)
	{
		if (game->sprites.door_frames[i].img.img)
			mlx_destroy_image(game->mlx,
				game->sprites.door_frames[i].img.img);
		i++;
	}
	free(game->sprites.door_frames);
}

t_door	*get_door_at(t_game *game, int x, int y)
{
	int	i;

	i = 0;
	while (i < game->doors)
	{
		if (game->door[i].x == x && game->door[i].y == y)
			return (&game->door[i]);
		i++;
	}
	return (NULL);
}

char	get_char_at(char **grid, int y, int x)
{
	if (y < 0 || !grid[y] || x < 0 || x >= (int)ft_strlen(grid[y]))
		return (' ');
	return (grid[y][x]);
}

bool	is_walkable(char c)
{
	return (c == '0' || c == 'N' || c == 'S' || c == 'W' || c == 'E');
}
