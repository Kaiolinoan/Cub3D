/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstclear.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kemontei <kemontei@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/21 19:10:42 by kemontei          #+#    #+#             */
/*   Updated: 2025/06/24 16:56:42 by kemontei         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	ft_lstclear(t_list **lst, void (*del)(void *))
{
	t_list	*current;
	t_list	*next;

	if (!del || !lst || !*lst)
		return ;
	current = *lst;
	while (current)
	{
		next = current->next;
		ft_lstdelone(current, (del));
		current = next;
	}
	*lst = NULL;
}
