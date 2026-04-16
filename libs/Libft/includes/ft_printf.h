/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kemontei <kemontei@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/01 16:37:41 by kemontei          #+#    #+#             */
/*   Updated: 2025/06/25 17:13:03 by kemontei         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_PRINTF_H
# define FT_PRINTF_H

# include <stdarg.h>

// Conversion functions
int		ft_printf(const char *str, ...);
int		ft_printfchar(int c);
int		ft_printfstring(const char *str);
int		ft_printfdecimal(int num);
int		ft_printfunsigned(unsigned int num);
int		ft_printfhex(unsigned int num, char c);
int		ft_printfpointer(void *ptr);

// Helper functions
char	*ft_uitoa(unsigned int n);
char	*ft_itoa_base(unsigned int num, char *base);
char	*ft_ulitoa_base(unsigned long int num, char *base);

#endif