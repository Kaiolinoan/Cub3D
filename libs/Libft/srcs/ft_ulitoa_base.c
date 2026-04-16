/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_ulitoa_base.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kemontei <kemontei@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/03 22:26:56 by kemontei          #+#    #+#             */
/*   Updated: 2025/06/09 21:09:58 by kemontei         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"
#include "libft.h"

static size_t	num_length(unsigned long int n, size_t base_len)
{
	size_t	i;

	i = 0;
	if (n == 0)
		return (1);
	while (n > 0)
	{
		n /= base_len;
		i++;
	}
	return (i);
}

static void	ft_putnbrbase_str(unsigned long int n, size_t base_len, ...)
{
	va_list	args;
	char	*base;
	char	*str;
	size_t	*i;

	va_start(args, base_len);
	base = va_arg(args, char *);
	str = va_arg(args, char *);
	i = va_arg(args, size_t *);
	if (n >= base_len)
		ft_putnbrbase_str(n / base_len, base_len, base, str, i);
	str[(*i)++] = base[n % base_len];
	va_end(args);
}

char	*ft_ulitoa_base(unsigned long int num, char *base)
{
	char			*result;
	size_t			i;
	size_t			len;
	size_t			base_len;

	base_len = ft_strlen(base);
	len = num_length(num, base_len);
	result = malloc((len + 1) * sizeof(char));
	if (!result)
		return (NULL);
	i = 0;
	ft_putnbrbase_str(num, base_len, base, result, &i);
	result[i] = '\0';
	return (result);
}
