/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   door_frames.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kelle <kelle@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/22 00:00:00 by kelle             #+#    #+#             */
/*   Updated: 2026/06/23 04:15:14 by kelle            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

static char	*build_frame_path(int i)
{
	char	*index_str;
	char	*path;
	char	*str2;

	index_str = ft_itoa(i);
	if (!index_str)
		return (NULL);
	str2 = ft_strjoin_and_free(index_str, ".xpm");
	if (!str2)
		return (free(index_str), NULL);
	path = ft_strjoin_and_free(ft_strdup("assets/animation/door_"), str2);
	if (!path)
		return (free(str2), NULL);
	return (free(str2), path);
}

static bool	load_door_frame(t_game *game, t_texture *frame, int i)
{
	char	*path;

	path = build_frame_path(i);
	if (!path)
		return (print_error(ALLOC_ERR), false);
	if (!set_image_path(frame, path))
		return (free(path), false);
	free(path);
	if (!file_to_image(game->mlx, frame))
		return (false);
	if (!get_dir_img_address(frame))
		return (mlx_destroy_image(game->mlx, frame->img.img),
			frame->img.img = NULL, false);
	return (true);
}

bool	load_door_frames(t_game *game, t_sprites *sprites)
{
	int	i;

	sprites->door_frames = ft_calloc(DOOR_ANIM_FRAMES, sizeof(t_texture));
	if (!sprites->door_frames)
		return (print_error(ALLOC_ERR), false);
	i = 0;
	while (i < DOOR_ANIM_FRAMES)
	{
		if (!load_door_frame(game, &sprites->door_frames[i], i))
		{
			clear_door_frames(game->mlx, sprites->door_frames, i);
			sprites->door_frames = NULL;
			return (false);
		}
		i++;
	}
	return (true);
}
