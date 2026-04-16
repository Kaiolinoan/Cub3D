/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_itoa.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kemontei <kemontei@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/09 14:38:55 by kemontei          #+#    #+#             */
/*   Updated: 2025/06/24 16:56:26 by kemontei         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static size_t	num_length(int n)
{
	size_t	i;

	i = 0;
	if (n == 0)
		i++;
	if (n < 0)
	{
		n *= -1;
		i++;
	}
	while (n > 0)
	{
		n /= 10;
		i++;
	}
	return (i);
}

static void	ft_putnbr_str(int n, char *str, size_t *i)
{
	if (n >= 10)
		ft_putnbr_str(n / 10, str, i);
	str[(*i)++] = (n % 10) + '0';
}

char	*ft_itoa(int n)
{
	char	*result;
	size_t	i;
	size_t	len;

	i = 0;
	if (n == -2147483648)
		return (ft_strdup("-2147483648"));
	len = num_length(n);
	result = malloc((len + 1) * sizeof(char));
	if (!result)
		return (NULL);
	if (n < 0)
	{
		result[i++] = '-';
		n *= -1;
	}
	ft_putnbr_str(n, result, &i);
	result[i] = '\0';
	return (result);
}
