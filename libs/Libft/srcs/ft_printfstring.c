/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printfstring.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kemontei <kemontei@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/01 17:36:33 by kemontei          #+#    #+#             */
/*   Updated: 2025/05/27 16:26:36 by kemontei         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"
#include "libft.h"

int	ft_printfstring(const char *str)
{
	int	count;

	count = 0;
	if (!str)
	{
		count += (write(1, "(null)", 6));
		return (count);
	}
	count += write(1, str, ft_strlen(str));
	return (count);
}
