/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   print_str.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klino-an <klino-an@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/29 16:58:47 by klino-an          #+#    #+#             */
/*   Updated: 2025/06/06 13:35:47 by klino-an         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_dprintf.h"

int	print_str_d(const char *str, int fd)
{
	int		count;

	count = 0;
	if (!str)
	{
		count += write(fd, "(null)", 6);
		return (count);
	}
	count += write(fd, str, ft_strlen2_d(str));
	return (count);
}
