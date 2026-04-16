/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klino-an <klino-an@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/25 20:23:33 by klino-an          #+#    #+#             */
/*   Updated: 2025/06/29 18:12:37 by klino-an         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_DPRINTF_H
# define FT_DPRINTF_H

# include <stdarg.h>
# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <stddef.h>

int		ft_dprintf(int fd, const char *str, ...);
int		print_char_d(int c, int fd);
int		print_str_d(const char *str, int fd);
int		print_pointer_d(void *ptr, int fd);
int		print_numbers_d(int nb, int fd);
int		print_unsigned_d(unsigned int nb, int fd);
int		print_hex_d(unsigned long nb, const char *hex, int fd);
char	*itoa_printf_d(int n);
size_t	ft_strlen2_d(const char *str);

#endif