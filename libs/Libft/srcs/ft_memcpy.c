/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memcpy.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kemontei <kemontei@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/09 14:39:37 by kemontei          #+#    #+#             */
/*   Updated: 2025/04/09 14:39:37 by kemontei         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_memcpy(void *dest, const void *src, size_t n)
{
	size_t			i;
	unsigned char	*dp;
	unsigned char	*sp;

	if (!dest || !src)
		return (NULL);
	if (!n || dest == src)
		return (dest);
	dp = dest;
	sp = (unsigned char *)src;
	i = 0;
	while (i < n)
	{
		dp[i] = sp[i];
		i++;
	}
	return (dest);
}
