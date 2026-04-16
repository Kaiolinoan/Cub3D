/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line_bonus.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kemontei <kemontei@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/14 19:55:29 by kemontei          #+#    #+#             */
/*   Updated: 2025/06/24 16:59:58 by kemontei         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line_bonus.h"

char	*get_next_line(int fd)
{
	static char	buffer[FOPEN_MAX][BUFFER_SIZE + 1];
	char		*line;
	ssize_t		chars_read;

	if (fd < 0 || fd >= FOPEN_MAX || BUFFER_SIZE <= 0)
		return (NULL);
	line = NULL;
	while (1)
	{
		if (buffer[fd][0] == '\0')
		{
			chars_read = read(fd, buffer[fd], BUFFER_SIZE);
			if (chars_read < 0)
				return (free(line), NULL);
			if (chars_read == 0)
				return (line);
			buffer[fd][chars_read] = '\0';
		}
		line = gnl_strjoin(line, buffer[fd]);
		if (!line || gnl_strchr(buffer[fd], '\n'))
			break ;
		buffer[fd][0] = '\0';
	}
	update_buffer(buffer[fd]);
	return (line);
}
