/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: psalame <psalame@student.42angouleme.fr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/01 15:10:14 by psalame           #+#    #+#             */
/*   Updated: 2024/08/01 18:06:37 by psalame          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ping.h"

int	main(int ac, char **arguments)
{
	arguments++;
	int		flags = get_flags(&arguments);
	printf("flags: %d\ncurrent argument: %s\n", flags, *arguments);

	return (0);
}