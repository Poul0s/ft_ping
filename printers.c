/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   printers.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: psalame <psalame@student.42angouleme.fr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/06 20:07:51 by psalame           #+#    #+#             */
/*   Updated: 2025/11/06 20:27:26 by psalame          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <math.h>
#include "ping.h"

void	show_help(char *ex_name) {
	printf("\
Usage: %s [OPTION...] HOST\n\
\n\
Send ICMP ECHO_REQUEST packets to network hosts.\n\
\n\
 Options valid for all request type:\n\
\n\
  -c, --count=NUMBER     stop after sending NUMBER packets\n\
  -i, --interval=NUMBER  wait NUMBER seconds between sending each packet\n\
      --ttl=N            specify N as time-to-live\n\
  -v, --verbose          verbose output\n\
  -f, --flood            flood ping (root only)\n\
  -s, --size=NUMBER      send NUMBER data octets\n\
\n\
  -?, --help             give this help list\n", ex_name);
}

void	print_usage(char *ex_name) {
	fprintf(stderr, "Try '%s --help' for more information.\n", ex_name);
}

void print_stats(struct ping_stats stats) {
	printf("--- Ping statistics ---\n");
	printf("%d packets transmitted, %d packets received, %d%% packet loss\n",
		stats.packets_sent,
		stats.packets_received,
		(int)(((float)(stats.packets_sent - stats.packets_received) / stats.packets_sent) * 100)
	);
	if (stats.min_time >= 0) {
		// round-trip min/avg/max/stddev = 24.267/24.764/25.261/0.497 ms
		double avg_time = stats.total_time / stats.packets_received_timed;
		
		double stddev = 0.0;

		if (stats.packets_received_timed > 1) {
			// σ = √(Σ(x² / N) - μ²) (écart-type de population)
			double mean_sq = stats.total_squared / stats.packets_received_timed;
			stddev = sqrt(mean_sq - (avg_time * avg_time));
		}
		

		printf("round-trip min/avg/max/stddev = %.3f/%.3f/%.3f/%.3f ms\n",
			stats.min_time,
			avg_time,
			stats.max_time,
			stddev
		);
	}
}