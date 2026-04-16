/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klino-an <klino-an@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/01 12:23:45 by klino-an          #+#    #+#             */
/*   Updated: 2025/06/29 18:12:37 by klino-an         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include <string.h>

static size_t	count_nb(int n)
{
	size_t	count;
	int		nb;

	nb = n;
	count = 0;
	if (nb < 0)
	{
		nb *= -1;
		count++;
	}
	while (nb >= 10)
	{
		nb /= 10;
		count++;
	}
	count++;
	return (count);
}

static char	*put_in_string(int n, char *str, size_t counter)
{
	size_t	counter2;

	counter2 = counter;
	if (n < 0)
	{
		n *= -1;
		str[0] = '-';
	}
	else if (n == 0)
		str[0] = '0';
	while (n > 0)
	{
		str[--counter] = (n % 10) + 48;
		n /= 10;
	}
	str[counter2] = '\0';
	return (str);
}

size_t	ft_strlen2_d(const char *str)
{
	size_t	i;

	if (!str)
		return (-1);
	i = 0;
	while (str[i])
		i++;
	return (i);
}

static char	*ft_strdup(const char *s)
{
	int		i;
	char	*p;

	i = 0;
	p = (char *)malloc(sizeof(char) * ft_strlen2_d(s) + 1);
	if (!p)
		return (NULL);
	while (s[i])
	{
		p[i] = s[i];
		i++;
	}
	p[i] = '\0';
	return (p);
}

char	*itoa_printf_d(int n)
{
	char	*str;
	size_t	counter;

	if (n == -2147483648)
		return (ft_strdup("-2147483648"));
	counter = count_nb(n);
	str = (char *)malloc((counter + 1) * sizeof(char));
	if (!str)
		return (NULL);
	put_in_string(n, str, counter);
	return (str);
}
