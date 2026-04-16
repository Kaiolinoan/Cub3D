/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strdup.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kemontei <kemontei@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/09 14:41:29 by kemontei          #+#    #+#             */
/*   Updated: 2025/04/09 14:41:29 by kemontei         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strdup(const char *s)
{
	size_t	arrlen;
	char	*arr;

	if (!s)
		return (NULL);
	arrlen = ft_strlen(s) + 1;
	arr = malloc(arrlen * sizeof(char));
	if (!arr)
		return (NULL);
	ft_strlcpy(arr, s, arrlen);
	return (arr);
}
