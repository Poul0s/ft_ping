/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: psalame <psalame@student.42angouleme.fr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/04 12:52:59 by psalame           #+#    #+#             */
/*   Updated: 2024/08/04 20:13:58 by psalame          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>

// #include <unistd.h>
// #include <getopt.h>
// int	main(int ac, char **av)
// {
// 	int	opt;
// 	int	optIndex = -1;
// 	const struct option parameters[] = {
// 		{"add", no_argument, 0, 'a'},
// 		{"create", required_argument, 0, 'c'},
// 		{0, 0, 0, 0}
// 	};

// 	while ((opt = getopt_long(ac, av, "?abc:c", parameters, &optIndex)) != -1)
// 	// while ((opt = getopt(ac, av, "?abc:")) != -1)
// 	{
// 		switch (opt)
// 		{
// 			case 'a':
// 				printf("Got argument 'a' with parameter '%s' and optIndex '%d'\n", optarg, optIndex);
// 				break;
// 			case 'b':
// 				printf("Got argument 'b' with parameter '%s' and optIndex '%d'\n", optarg, optIndex);
// 				break;
// 			case 'c':
// 				printf("Got argument 'c' with parameter '%s' and optIndex '%d'\n", optarg, optIndex);
// 				break;
// 			case '?':
// 				printf("Got bargument '%c' (%d) and optIndex '%d'\n", optopt, optopt, optIndex);
// 				break;
// 			default:
// 				printf("bad usage\n");
// 				break;
// 		}
// 	}
// 	for (int i = optind; i < ac; i++)
// 		printf("Argument '%s'\n", av[i]);
// 	return 0;
// }

#include "libs/ft_getopt/includes/getopt.h"

int	main(int ac, char **av)
{
	t_optres	opt = {-1, 0, 1};
	const t_option parameters[] = {
		{"add", 0, 'a'},
		{"create", 1, 'c'},
		{0, 0, 0}
	};

	while ((opt = ft_getopt_long(ac, av, "?ac:b", parameters)).val != -1)
	// while ((opt = ft_getopt(ac, av, "?ac:b")).val != -1)
	{
		switch (opt.val)
		{
			case 'a':
				printf("Got argument 'a' with parameter '%s'\n", opt.arg);
				break;
			case 'b':
				printf("Got argument 'b' with parameter '%s'\n", opt.arg);
				break;
			case 'c':
				printf("Got argument 'c' with parameter '%s'\n", opt.arg);
				break;
			case '?':
				printf("Got argument '?' with parameter '%s'\n", opt.arg);
				break;
			case 0:
				printf("Got bargument '%c' (%d)\n", opt.val, opt.val);
				break;
			default:
				printf("bad usage\n");
				break;
		}
	}
	for (int i = opt.next_argument; i < ac; i++)
		printf("Argument '%s'\n", av[i]);
	return 0;
}