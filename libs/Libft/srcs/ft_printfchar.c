/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printfchar.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kemontei <kemontei@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/01 16:46:05 by kemontei          #+#    #+#             */
/*   Updated: 2025/05/27 16:24:00 by kemontei         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"
#include "libft.h"

int	ft_printfchar(int c)
{
	unsigned char	res;
	int				count;

	count = 0;
	res = (unsigned char)c;
	count += write(1, &res, 1);
	return (count);
}
