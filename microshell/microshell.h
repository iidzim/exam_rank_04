/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   microshell.h                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: iidzim <iidzim@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/17 14:58:21 by iidzim            #+#    #+#             */
/*   Updated: 2021/10/25 16:24:30 by iidzim           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MICROSHELL_H
# define MICROSHELL_H

# include <unistd.h>
# include <stdlib.h>
# include <string.h>

typedef struct s_cmd{
	char	**tokens;
	int		pipe[2];
	
	struct s_cmd *next;
}               t_cmd;

int ft_strlen(char *str);
void print_error(int i, char *path)
int cd_builtin(char **argv);

#endif
