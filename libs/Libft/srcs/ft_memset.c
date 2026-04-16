/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memset.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kemontei <kemontei@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/09 14:39:52 by kemontei          #+#    #+#             */
/*   Updated: 2025/04/09 14:39:52 by kemontei         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_memset(void *s, int c, size_t n)
{
	size_t			i;
	unsigned char	*ptr;
	unsigned char	value;

	if (!s)
		return (NULL);
	ptr = (unsigned char *)s;
	value = (unsigned char)c;
	i = 0;
	while (i < n)
		ptr[i++] = value;
	return (s);
}
