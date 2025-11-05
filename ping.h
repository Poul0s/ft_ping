/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ping.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: psalame <psalame@student.42angouleme.fr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/01 15:09:51 by psalame           #+#    #+#             */
/*   Updated: 2025/11/05 16:04:48 by psalame          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PING_H
# define PING_H
# include <stdbool.h>
# include <stdio.h>
# include <sys/socket.h>
# include <netinet/in.h>
# include "libs/ft_getopt/includes/getopt.h"

enum flags {
	FLAG_HELP		=	1 << 0, // -?, --help
	FLAG_VERBOSE	=	1 << 1, // -v, --verbose
	
	// ...
	
	UNKNOWN_FLAG	=	1 << 2
};

struct target_host {
	char *host;
	int socket;
	struct sockaddr_in addr;
};

int	get_flags(int ac, char **av);

#endif