/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: iidzim <iidzim@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/17 15:31:32 by iidzim            #+#    #+#             */
/*   Updated: 2021/10/25 16:02:03 by iidzim           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "microshell.h"

int ft_strlen(char *str)
{
	int i;

	i = 0;
	while (str[i] != '\0')
		i++;
	return (i);
}

void print_error(int i, char *path)
{
	if (i == 0)
		write(2, "error: fatal\n", 13);
	if (i == 1)
	{
		write(2, "error: cannot execute ", 22);
		write(2, *path, ft_strlen(path));
		write(2, "\n", 1);
	}
	exit(EXIT_FAILURE);
}


