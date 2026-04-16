/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memmove.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kemontei <kemontei@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/09 14:39:46 by kemontei          #+#    #+#             */
/*   Updated: 2025/04/09 14:39:46 by kemontei         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_memmove(void *dest, const void *src, size_t n)
{
	size_t				i;
	unsigned char		*dp;
	const unsigned char	*sp;

	if (!dest || !src)
		return (NULL);
	if (!n)
		return (dest);
	dp = (unsigned char *)dest;
	sp = (const unsigned char *)src;
	if (dp > sp && dp < sp + n)
	{
		i = n;
		while (i--)
			dp[i] = sp[i];
	}
	else
		ft_memcpy(dp, sp, n);
	return (dest);
}
