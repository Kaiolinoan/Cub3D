/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstmap.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kemontei <kemontei@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/23 12:23:39 by marvin            #+#    #+#             */
/*   Updated: 2025/06/24 16:57:01 by kemontei         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

t_list	*ft_lstmap(t_list *lst, void *(*f)(void *), void (*del)(void *))
{
	t_list	*newhead;
	t_list	*newnode;
	void	*newcontent;

	if (!lst || !f || !del)
		return (NULL);
	newhead = NULL;
	while (lst)
	{
		newcontent = (f)(lst->content);
		newnode = ft_lstnew(newcontent);
		if (!newcontent || !newnode)
		{
			(del)(newcontent);
			ft_lstclear(&newhead, del);
			free(newnode);
			return (NULL);
		}
		ft_lstadd_back(&newhead, newnode);
		lst = lst->next;
	}
	return (newhead);
}
