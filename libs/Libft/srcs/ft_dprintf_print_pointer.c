/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   print_pointer.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klino-an <klino-an@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/29 16:58:44 by klino-an          #+#    #+#             */
/*   Updated: 2025/05/06 10:17:00 by klino-an         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_dprintf.h"

int	print_pointer_d(void *ptr, int fd)
{
	int					count;
	unsigned long int	address;

	count = 0;
	if (!ptr)
	{
		count += write(fd, "(nil)", 5);
		return (count);
	}
	address = (unsigned long int)ptr;
	count += write(fd, "0x", 2);
	count += print_hex_d(address, "0123456789abcdef", fd);
	return (count);
}
