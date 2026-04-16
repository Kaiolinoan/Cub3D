/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_uitoa.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kemontei <kemontei@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/03 20:47:27 by kemontei          #+#    #+#             */
/*   Updated: 2025/05/27 15:13:34 by kemontei         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"
#include "libft.h"

static size_t	num_length(unsigned int n)
{
	size_t	i;

	i = 0;
	if (n == 0)
		return (1);
	while (n > 0)
	{
		n /= 10;
		i++;
	}
	return (i);
}

static void	ft_putnbr_str(unsigned int n, char *str, size_t *i)
{
	if (n >= 10)
		ft_putnbr_str(n / 10, str, i);
	str[(*i)++] = (n % 10) + '0';
}

char	*ft_uitoa(unsigned int n)
{
	char	*result;
	size_t	i;
	size_t	len;

	len = num_length(n);
	result = malloc((len + 1) * sizeof(char));
	if (!result)
		return (NULL);
	i = 0;
	ft_putnbr_str(n, result, &i);
	result[i] = '\0';
	return (result);
}
