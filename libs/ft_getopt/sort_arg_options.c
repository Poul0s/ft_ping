/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sort_arg_options.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: psalame <psalame@student.42angouleme.fr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/04 14:39:15 by psalame           #+#    #+#             */
/*   Updated: 2024/08/04 19:50:35 by psalame          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "getopt_Int.h"

inline bool	is_arg_option(char * arg)
{
	return (arg[0] == '-' && arg[1] != 0 && (arg[1] != '-' || arg[2] != 0));
}

static void	move_argument_at_pos(char ** av, int src_i, int dest_i)
{
	char	*argument = av[src_i];

	while (src_i != dest_i)
	{
		av[src_i] = av[src_i - 1];
		src_i--;
	}
	av[src_i] = argument;
}

void	sort_next_argument(int ac, char ** av, int i)
{
	if (!is_arg_option(av[i]))
	{
		int	j = i + 1;
		while (j < ac)
		{
			if (is_arg_option(av[j]))
				move_argument_at_pos(av, j, i);
			/// TODO HAVE TO MOVE ARGUMENT VALUE IF OPT has_arg
			j++;
		}
	}
}
