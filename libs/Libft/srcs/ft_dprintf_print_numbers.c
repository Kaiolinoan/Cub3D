/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   print_numbers.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klino-an <klino-an@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/29 16:58:42 by klino-an          #+#    #+#             */
/*   Updated: 2025/06/29 18:13:01 by klino-an         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_dprintf.h"

int	print_numbers_d(int nb, int fd)
{
	int		count;
	char	*str;

	count = 0;
	str = itoa_printf_d(nb);
	if (!str)
		return (-1);
	count += write(fd, str, ft_strlen2_d(str));
	free(str);
	return (count);
}

int	print_unsigned_d(unsigned int nb, int fd)
{
	int		count;
	char	c;

	count = 0;
	if (nb >= 10)
		count += print_unsigned_d(nb / 10, 2);
	c = nb % 10 + '0';
	count += write (fd, &c, 1);
	return (count);
}
