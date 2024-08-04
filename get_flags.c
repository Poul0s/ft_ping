/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_flags.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: psalame <psalame@student.42angouleme.fr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/01 15:58:52 by psalame           #+#    #+#             */
/*   Updated: 2024/08/04 14:12:52 by psalame          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ping.h"
#include <stdio.h>
#include <getopt.h>

int	get_flags(int ac, char **av)
{
	int	opt;
	int	flags = 0;
	int	optIndex = -1;
	const struct option parameters[] = {
		{"verbose", no_argument, 0, 'v'},
		{"help", no_argument, 0, 'h'},
		{0, 0, 0, 0}
	};

	while ((opt = getopt_long(ac, av, "vh", parameters, &optIndex)) != -1)
	{
		switch (opt)
		{
			case 'v':
				flags |= FLAG_VERBOSE;
				break;
			case 'h':
				flags |= FLAG_HELP;
				break;
		}
	}
	
}
