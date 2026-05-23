/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_rgb.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kelle <kelle@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/23 02:47:46 by kelle             #+#    #+#             */
/*   Updated: 2026/05/23 02:48:40 by kelle            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

static int	comma_count(char *str)
{
	size_t	i;
	size_t	count;

	i = 0;
	count = 0;
	while (str[i])
	{
		if (str[i] == ',')
			count++;
		i++;
	}
	return (count);
}

static bool	fill_rgb(t_game *game, char **rgb, bool is_floor)
{
	if (is_floor)
	{
		game->map->floor = ft_calloc(1, sizeof(t_rgb));
		if (!game->map->floor)
			return (print_error(ALLOC_ERR), false);
		game->map->floor->r = (unsigned char)ft_atoi(rgb[0]);
		game->map->floor->g = (unsigned char)ft_atoi(rgb[1]);
		game->map->floor->b = (unsigned char)ft_atoi(rgb[2]);
	}
	else
	{
		game->map->ceiling = ft_calloc(1, sizeof(t_rgb));
		if (!game->map->ceiling)
			return (print_error(ALLOC_ERR), false);
		game->map->ceiling->r = (unsigned char)ft_atoi(rgb[0]);
		game->map->ceiling->g = (unsigned char)ft_atoi(rgb[1]);
		game->map->ceiling->b = (unsigned char)ft_atoi(rgb[2]);
	}
	return (true);
}

static bool	parse_and_fill_rgb(t_game *game, char **coord, bool is_floor)
{
	char	**rgb;
	char	*str;
	size_t	i;
	size_t	j;

	str = ft_strtrim(coord[1], "\n");
	if (!str)
		return (print_error(ALLOC_ERR), false);
	rgb = ft_split(str, ',');
	if (!rgb || !*rgb || !rgb[1] || !rgb[2] || rgb[3] || comma_count(str) != 2)
		return (free(str), print_error("Invalid RGB"), clear_matriz(rgb), 0);
	free(str);
	i = -1;
	while (rgb[++i])
	{
		j = -1;
		while (rgb[i][++j])
			if (!ft_isdigit(rgb[i][j]) && rgb[i][j])
				return (print_error(RGB_DIGIT_ERR), clear_matriz(rgb), false);
		if (ft_atoi(rgb[i]) < 0 || ft_atoi(rgb[i]) > 255 || *rgb[i] == '\n')
			return (print_error(RGB_RANGE_ERR), clear_matriz(rgb), false);
	}
	if (!fill_rgb(game, rgb, is_floor))
		return (clear_matriz(rgb), false);
	return (clear_matriz(rgb), true);
}

int	filter_rgb(char **arr, t_game *game)
{
	if (!ft_strncmp(*arr, "C", 2))
	{
		if (!game->map->ceiling)
		{
			if (parse_and_fill_rgb(game, arr, false))
				return (1);
			return (-2);
		}
		return (print_error("Double definition of element"), -1);
	}
	if (!ft_strncmp(*arr, "F", 2))
	{
		if (!game->map->floor)
		{
			if (parse_and_fill_rgb(game, arr, true))
				return (1);
			return (-2);
		}
		return (print_error("Double definition of element"), -1);
	}
	return (0);
}
