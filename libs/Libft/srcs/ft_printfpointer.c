/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printfpointer.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kemontei <kemontei@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/03 20:35:13 by kemontei          #+#    #+#             */
/*   Updated: 2025/05/27 15:13:18 by kemontei         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"
#include "libft.h"

int	ft_printfpointer(void *ptr)
{
	int		count;
	char	*str;

	count = 0;
	if (!ptr)
	{
		count += write(1, "(nil)", 5);
		return (count);
	}
	str = ft_ulitoa_base((unsigned long int)ptr, "0123456789abcdef");
	if (!str)
		return (-1);
	count += write(1, "0x", 2);
	count += write(1, str, ft_strlen(str));
	free(str);
	return (count);
}
