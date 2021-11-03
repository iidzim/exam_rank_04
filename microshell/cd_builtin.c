/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd_builtin.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: iidzim <iidzim@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/17 15:18:51 by iidzim            #+#    #+#             */
/*   Updated: 2021/09/17 15:38:01 by iidzim           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "microshell.h"

int cd_builtin(char **argv)
{
    if (len(argv) > 2)
    {
        if (chdir(argv[1]) == -1)
        {
            write(2, "error: cd: cannot change directory to", 37);
            write(2, argv[1], ft_strlen(argv[1]));
            write(2, "\n", 1);
        }
    }
    else
        write(2, "error: cd: bad arguments\n", 25);
}
