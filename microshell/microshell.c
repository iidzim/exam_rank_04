/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   microshell.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: iidzim <iidzim@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/17 14:57:42 by iidzim            #+#    #+#             */
/*   Updated: 2021/10/25 16:24:44 by iidzim           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "microshell.h"

void parse(t_cmd *cmd, char *argv){
	if (!strcmp(argv, ";") && !strcmp(argv, "|") && !cmd)
		return (print_error(0, NULL));
		
}

int main(int argc, char **argv, char **env)
{
	t_cmd cmd;
	int i;

	i = 1;
	while (i < argc)
		parse(&cmd, argv[i++]);
	



	return (0);
}