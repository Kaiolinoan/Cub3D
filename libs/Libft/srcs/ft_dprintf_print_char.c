/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   print_char.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klino-an <klino-an@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/29 16:58:37 by klino-an          #+#    #+#             */
/*   Updated: 2025/05/06 10:13:46 by klino-an         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_dprintf.h"

int	print_char_d(int c, int fd)
{
	int				count;
	unsigned char	uc;

	uc = (unsigned char)c;
	count = 0;
	count += write(fd, &uc, 1);
	return (count);
}
