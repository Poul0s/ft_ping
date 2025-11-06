/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ping.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: psalame <psalame@student.42angouleme.fr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/01 15:09:51 by psalame           #+#    #+#             */
/*   Updated: 2025/11/06 20:15:52 by psalame          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PING_H
# define PING_H
# include <stdbool.h>
# include <stdio.h>
# include <sysexits.h>
# include <sys/socket.h>
# include <netinet/in.h>
# include "libs/ft_getopt/includes/getopt.h"
# include <stdlib.h>
# include <netinet/ip_icmp.h>
# define INTERVAL_DEFAULT 1000 // miliseconds
# define USER_MIN_INTERVAL_MS 200
# define PAYLOAD_SIZE 56
# define MAX_PACKET_SIZE (65535 - 60 - 76)

enum flags {
	FLAG_HELP		=	1 << 0, // -?, --help
	FLAG_VERBOSE	=	1 << 1, // -v, --verbose
	FLAG_TTL		=	1 << 3, // -t, --ttl
	FLAG_COUNT		=	1 << 4, // -c, --count
	FLAG_FLOOD		=	1 << 5, // -f, --flood
	FLAG_INTERVAL	=	1 << 6, // -i, --interval
	FLAG_SIZE		=	1 << 7, // -s, --size
	
	// ...
	
	UNKNOWN_FLAG	=	1 << 8
};

struct ping_opts {
	unsigned char	ttl;
	unsigned long	count;
	unsigned long	interval_ms;
	unsigned short	size;
};

struct target_host {
	char *host;
	int socket;
	struct sockaddr_in addr;
	struct icmphdr icmp_sent;
	size_t packet_size;
};

struct ping_stats {
	u_int16_t	packets_sent;
	u_int16_t	packets_received;
	u_int16_t	packets_received_timed;
	double		min_time;
	double		max_time;
	double		total_time;
	double		total_squared;
};


// get flags
int	get_flags(int ac, char **av, struct ping_opts *options);

// printers
void	show_help(char *ex_name);
void	print_usage(char *ex_name);
void	print_stats(struct ping_stats stats);



#endif