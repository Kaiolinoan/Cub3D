/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kemontei <kemontei@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/01 16:35:15 by kemontei          #+#    #+#             */
/*   Updated: 2025/06/24 16:58:19 by kemontei         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"
#include "libft.h"

static int	check_conversions(va_list args, char c)
{
	int	count;

	count = 0;
	if (c == 'c')
		count += ft_printfchar(va_arg(args, int));
	else if (c == 's')
		count += ft_printfstring(va_arg(args, const char *));
	else if (c == 'd' || c == 'i')
		count += ft_printfdecimal(va_arg(args, int));
	else if (c == 'u')
		count += ft_printfunsigned(va_arg(args, unsigned int));
	else if (c == 'x' || c == 'X')
		count += ft_printfhex(va_arg(args, unsigned int), c);
	else if (c == 'p')
		count += ft_printfpointer(va_arg(args, void *));
	else if (c == '%')
		count += write(1, "%", 1);
	return (count);
}

static int	handle_regular_chars(const char *str, size_t *i)
{
	int	count;

	count = 0;
	while (str[*i] && str[*i] != '%')
	{
		write(1, &str[(*i)++], 1);
		count++;
	}
	return (count);
}

static int	handle_conversions(const char *str, va_list args, size_t *i)
{
	int	count;

	count = 0;
	if (str[*i + 1])
	{
		count += check_conversions(args, str[*i + 1]);
		*i += 2;
	}
	else
		(*i)++;
	return (count);
}

int	ft_printf(const char *str, ...)
{
	va_list	args;
	int		count;
	size_t	i;

	if (!str || (str[0] == '%' && str[1] == '\0'))
		return (-1);
	count = 0;
	i = 0;
	va_start(args, str);
	while (str[i])
	{
		count += handle_regular_chars(str, &i);
		if (str[i] == '%')
			count += handle_conversions(str, args, &i);
	}
	va_end(args);
	return (count);
}
