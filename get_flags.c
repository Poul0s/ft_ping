/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_flags.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: psalame <psalame@student.42angouleme.fr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/01 15:58:52 by psalame           #+#    #+#             */
/*   Updated: 2025/11/05 15:00:40 by psalame          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ping.h"
#include <stdio.h>

int	get_flags(int ac, char **av)
{
	struct s_optres	opt;
	int	flags = 0;
	const struct s_option parameters[] = {
		{"verbose", 0, 'v'},
		{"help", 0, '?'},
		{0, 0, 0}
	};

	while ((opt = ft_getopt_long(ac, av, "v?", parameters)).val != -1)
	{
		printf("Opt %c (%d)\n", opt.val, opt.val);
		switch (opt.val)
		{
			case 'v':
				flags |= FLAG_VERBOSE;
				break;
			case '?':
				flags |= FLAG_HELP;
				break;
		}
	}
	return flags;
}
