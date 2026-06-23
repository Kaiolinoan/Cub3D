/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   door.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kelle <kelle@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/22 00:00:00 by kelle             #+#    #+#             */
/*   Updated: 2026/06/23 04:15:14 by kelle            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

static bool	player_near_door(t_player *player, t_door *door, double range)
{
	double	dx;
	double	dy;

	dx = player->player_x - (door->x + 0.5);
	dy = player->player_y - (door->y + 0.5);
	return (sqrt(dx * dx + dy * dy) <= range);
}

static void	advance_door_animation(t_door *door)
{
	if (door->state == DOOR_OPENING)
	{
		door->frame++;
		if (door->frame >= DOOR_ANIM_FRAMES)
		{
			door->frame = DOOR_ANIM_FRAMES;
			door->state = DOOR_OPEN;
		}
	}
	else if (door->state == DOOR_CLOSING)
	{
		door->frame--;
		if (door->frame <= 0)
		{
			door->frame = 0;
			door->state = DOOR_CLOSED;
		}
	}
}

static void	update_door_state(t_door *door, bool near)
{
	if (near && (door->state == DOOR_CLOSED || door->state == DOOR_CLOSING))
	{
		door->state = DOOR_OPENING;
		play_sound(DOOR_OPEN_SOUND);
	}
	else if (!near && (door->state == DOOR_OPEN || door->state == DOOR_OPENING))
	{
		door->state = DOOR_CLOSING;
		play_sound(DOOR_CLOSE_SOUND);
	}
	advance_door_animation(door);
}

void	update_doors(t_game *game)
{
	int		i;
	bool	near;

	i = 0;
	while (i < game->doors)
	{
		near = player_near_door(&game->player, &game->door[i], DOOR_RANGE);
		update_door_state(&game->door[i], near);
		i++;
	}
}

bool	is_door_blocking(t_game *game, int x, int y)
{
	t_door	*door;

	door = get_door_at(game, x, y);
	if (!door)
		return (false);
	return (door->state != DOOR_OPEN);
}
