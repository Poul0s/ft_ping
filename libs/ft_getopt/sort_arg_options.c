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

// bool	is_arg_options_sorted(int ac, char ** av)
// {
// 	int	i = 1;
// 	while (i < ac && is_arg_option(av[i]))
// 		i++;
// 	while (++i < ac)
// 		if (is_arg_option(av[i]))
// 			return false;
// 	return true;
// }

// void	sort_arg_options(int ac, char ** av)
// {
// 	int	i = 1;
// 	while (i < ac && is_arg_option(av[i]))
// 		i++;
// 	int	last_option_position = i++;
// 	while (i < ac)
// 	{
// 		if (is_arg_option(av[i]))
// 			move_argument_at_pos(av, i, last_option_position++);
// 		i++;
// 	}
// }

void	sort_next_argument(int ac, char ** av, int i)
{
	if (!is_arg_option(av[i]))
	{
		int	j = i + 1;
		while (j < ac)
		{
			if (is_arg_option(av[j]))
				move_argument_at_pos(av, j, i);
			j++;
		}
	}
}
