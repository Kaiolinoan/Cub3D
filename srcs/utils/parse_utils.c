#include "cub3d.h"

size_t	ft_strlen_without_space(const char *s)
{
	size_t		i;
	size_t		count;

	i = 0;
	count = 0;
	if (!s)
		return (0);
	while (s[i])
    {
        if (s[i] != 32 || s[i] != 9)
		    count++;
        i++;        
    }
	return (count);
}

char	*ft_strjoin3(char *s1, char *s2)
{
	char	*str;
	size_t	i;
	size_t	j;
	size_t	k;
    int size;

    size = ft_strlen_without_space(s1) + ft_strlen_without_space(s2) + 1;
	str = ft_calloc(1, size);
	if (!str)
		return (print_error(ALLOC_ERR), NULL);
	i = 0;
    k = 0;
	j = 0;
	while (s1 && s1[i])
	{
        while(s1[i] == 32 || s1[i] == 9 )
            i++;
		str[k++] = s1[i++];
	}
	while (s2 && s2[j])
	{
		while(s2[j] == 32 || s2[j] == 9)
			j++;
		str[i++] = s2[j++];
	} 
	return (str[i] = '\0', str);
}
