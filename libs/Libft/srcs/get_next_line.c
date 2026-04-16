/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kemontei <kemontei@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/07 16:06:35 by kemontei          #+#    #+#             */
/*   Updated: 2025/06/24 17:00:08 by kemontei         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"

char	*get_next_line(int fd)
{
	static char	buffer[BUFFER_SIZE + 1];
	char		*line;
	ssize_t		chars_read;

	if (fd < 0 || BUFFER_SIZE <= 0)
		return (NULL);
	line = NULL;
	while (1)
	{
		if (buffer[0] == '\0')
		{
			chars_read = read(fd, buffer, BUFFER_SIZE);
			if (chars_read < 0)
				return (free(line), NULL);
			if (chars_read == 0)
				return (line);
			buffer[chars_read] = '\0';
		}
		line = gnl_strjoin(line, buffer);
		if (!line || gnl_strchr(buffer, '\n'))
			break ;
		buffer[0] = '\0';
	}
	update_buffer(buffer);
	return (line);
}
