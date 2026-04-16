/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   print_hex.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klino-an <klino-an@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/29 16:58:39 by klino-an          #+#    #+#             */
/*   Updated: 2025/05/06 10:14:36 by klino-an         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_dprintf.h"

int	print_hex_d(unsigned long nb, const char *hex, int fd)
{
	int	count;

	count = 0;
	if (nb >= 16)
		count += print_hex_d(nb / 16, hex, 2);
	count += write(fd, &hex[nb % 16], 1);
	return (count);
}
