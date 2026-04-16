/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_substr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kemontei <kemontei@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/24 20:28:03 by kemontei          #+#    #+#             */
/*   Updated: 2025/04/24 20:28:03 by kemontei         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_substr(char const *s, unsigned int start, size_t len)
{
	size_t	str_len;
	char	*ptr;

	if (!s)
		return (NULL);
	str_len = ft_strlen(s);
	if (!len || start >= str_len)
		return (ft_strdup(""));
	if (len > str_len - start)
		len = str_len - start;
	ptr = malloc((len + 1) * sizeof(char));
	if (!ptr)
		return (NULL);
	ft_strlcpy(ptr, &s[start], len + 1);
	return (ptr);
}
