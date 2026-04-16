/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printfhex.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kemontei <kemontei@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/03 19:13:47 by kemontei          #+#    #+#             */
/*   Updated: 2025/05/05 15:54:40 by kemontei         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"
#include "libft.h"

int	ft_printfhex(unsigned int num, char c)
{
	int		count;
	char	*str;

	count = 0;
	if (c == 'x')
		str = ft_itoa_base(num, "0123456789abcdef");
	else
		str = ft_itoa_base(num, "0123456789ABCDEF");
	if (!str)
		return (-1);
	count += write(1, str, ft_strlen(str));
	free (str);
	return (count);
}
