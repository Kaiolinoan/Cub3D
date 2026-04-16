/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_calloc.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kemontei <kemontei@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/09 14:34:32 by kemontei          #+#    #+#             */
/*   Updated: 2025/04/09 14:34:32 by kemontei         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_calloc(size_t num, size_t size)
{
	void	*p;

	if (!num || !size)
		return (p = malloc(0));
	if (num > ((size_t) - 1) / size)
		return (NULL);
	else
		p = malloc(num * size);
	if (!p)
		return (NULL);
	ft_bzero(p, num * size);
	return (p);
}
/* 
int main()
{
	int *my_array;
	int *real_array;
	int num = 5;
	int size = sizeof(int);

	my_array = ft_calloc(num, size);

	if (my_array)
	{
		printf("\nMy ft_calloc: ");
		for (int i = 0; i < num; i++)
			printf("%d ", my_array[i]);
		printf("\n");
		free(my_array);
	}
	else
		printf("My ft_calloc failed to allocate memory.\n");

	real_array = (int *)calloc(num, size);

	if (real_array)
	{
		printf("\nOriginal calloc: ");
		for (int i = 0; i < num; i++)
			printf("%d ", real_array[i]);
		printf("\n\n");
		free(real_array);
	}
	else
		printf("Real calloc failed to allocate memory.\n");
	return 0;
}
*/
