/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: psalame <psalame@student.42angouleme.fr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/01 15:10:14 by psalame           #+#    #+#             */
/*   Updated: 2025/11/06 19:57:12 by psalame          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ping.h"
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <linux/time.h>
#include <poll.h>
#include <netdb.h>
#include <signal.h>
#include <math.h>

struct ping_stats stats = {0, 0, 0, -1.0f, 0.0f, 0.0f, 0.0f};

void	show_help(char *ex_name) {
	printf("\
Usage: %s [OPTION...] HOST\n\
\n\
Send ICMP ECHO_REQUEST packets to network hosts.\n\
\n\
 Options valid for all request type:\n\
\n\
  -v, --verbose  verbose output\n\
\n\
  -?, --help     give this help list\n", ex_name);
}

u_int16_t	checksum16(const void *data, size_t len) {
	const uint16_t *buf = data;
	uint32_t sum = 0;
	
	for (size_t i = 0; i < len / 2; i++) {
		sum += buf[i];
	}
	if (len % 2) {
		sum += ((const uint8_t *)data)[len - 1];
	}
	while (sum >> 16) {
		sum = (sum & 0xFFFF) + (sum >> 16);
	}
	return ~sum;
}

struct timespec send_packet(u_int16_t sequence, struct target_host *host, unsigned short packet_size) {
	const pid_t pid = getpid();
	
	char payload[packet_size];
	char packet[sizeof(host->icmp_sent) + packet_size];
	
	// Remplissage des champs variables
	host->icmp_sent.un.echo.id = htons(pid); // inetutils use getpid() as identifier
	host->icmp_sent.un.echo.sequence = htons(sequence);
	memcpy(packet, &host->icmp_sent, sizeof(host->icmp_sent));

	struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);
	if (packet_size != 0) {
		int wrote = snprintf(payload, packet_size, "%ld.%09ld", ts.tv_sec, ts.tv_nsec); // theoric max possible size : 20 + 1 + 9 = 30
		if (wrote < 0 || wrote >= (int) packet_size) {
			payload[0] = '\0';
		}
		memcpy(packet + sizeof(host->icmp_sent), payload, packet_size);
	}
	
	((struct icmphdr *)packet)->checksum = checksum16(packet, sizeof(packet));

	
	// Envoi du paquet
	stats.packets_sent++;
	ssize_t bytes_sent = sendto(host->socket, packet, sizeof(packet), 0,
		(struct sockaddr *)&host->addr, sizeof(host->addr));
	host->packet_size = bytes_sent;
	if (bytes_sent < 0) {
		perror("sending packet");
		exit(EXIT_FAILURE);
	}

	return ts;
}

void receive_packet(struct target_host *host, int flags, struct ping_opts *opts) {
	const pid_t pid = getpid();
	char recv_buffer[1024];
	struct sockaddr_in recv_addr;
	socklen_t addr_len = sizeof(recv_addr);
	ssize_t bytes_received = recvfrom(host->socket, recv_buffer, sizeof(recv_buffer), 0,
		(struct sockaddr *)&recv_addr, &addr_len);
		
	if (bytes_received < 0)
		return ;

	
	struct timespec ts_recv;
	clock_gettime(CLOCK_MONOTONIC, &ts_recv);
	struct iphdr *ip_header = (struct iphdr *)recv_buffer;
	size_t ip_header_len = ip_header->ihl * 4;
	struct icmphdr *recv_icmp = (struct icmphdr *)(recv_buffer + ip_header_len);
	char *recv_payload = recv_buffer + ip_header_len + sizeof(struct icmphdr);
	// recv_payload[bytes_received - ip_header_len - sizeof(struct icmphdr)] = '\0';

	if (recv_icmp->type == 11) {
		printf("%zd bytes from %s: Time to live exceeded\n",
			bytes_received - ip_header_len,
			inet_ntoa(recv_addr.sin_addr)
		);
		if (flags & FLAG_VERBOSE) {

			// IP header dump
			printf("IP Hdr Dump:\n");
			for (size_t i = 0; i < ip_header_len; i++) {
				printf(i % 2 == 0 ? " %02x" : "%02x", ((uint8_t *)ip_header)[i]);
			}
			printf("\n");
			printf("Vr HL TOS  Len   ID Flg  off TTL Pro  cks      Src             Dst        Data\n");
			printf("%2x %2lx  %2x %04x %x %3x %04d %3d  %02x %04x %15s %15s \n", 
				4, //ipv4
				ip_header_len / 4, //ip header length
				ip_header->tos, // type of service
				ntohs(ip_header->tot_len), // total length
				ntohs(ip_header->id), // identification
				ntohs(ip_header->frag_off) >> 13, // flags
				ntohs(ip_header->frag_off) & 0x1FFF, // fragment offset
				(flags & FLAG_TTL) ? opts->ttl : ip_header->ttl, // ttl
				ip_header->protocol, // protocol
				ntohs(ip_header->check), // checksum
				inet_ntoa(*(struct in_addr *)&ip_header->saddr), // source address
				inet_ntoa(*(struct in_addr *)&ip_header->daddr)); // dest address
			
			// Sent ICMP header dump
			
			printf("ICMP: type %d, code %d, size %ld, id 0x%04x, seq 0x%04x\n",
				host->icmp_sent.type,
				host->icmp_sent.code,
				host->packet_size,
				ntohs(host->icmp_sent.un.echo.id),
				ntohs(host->icmp_sent.un.echo.sequence)
			);
		}
		return 0;
	}

	if (recv_icmp->un.echo.id != htons(pid) || recv_icmp->type != 0) {
		// not for us, ignore
		return 0;
	}
	stats.packets_received++;

	if (flags & FLAG_FLOOD) {
		putchar('\b');
	} else {
		printf("%zd bytes from %s: icmp_seq=%d ttl=%d",
			bytes_received - ip_header_len,
			inet_ntoa(recv_addr.sin_addr),
			ntohs(recv_icmp->un.echo.sequence),
			(flags & FLAG_TTL) ? opts->ttl - (64 - ip_header->ttl) : ip_header->ttl
		);
	}
	char *separator = strchr(recv_payload, '.');
	if (!separator) {
		printf("\n");
	} else {
		*separator = '\0';
		long sec_sent = atol(recv_payload);
		long nsec_sent = atol(separator + 1);
		float elapsed_ms = (ts_recv.tv_sec - sec_sent) * 1000 + ((float)ts_recv.tv_nsec - nsec_sent) / 1000000;

		if (stats.min_time < 0 || elapsed_ms < stats.min_time)
			stats.min_time = elapsed_ms;
		if (elapsed_ms > stats.max_time)
			stats.max_time = elapsed_ms;
		stats.total_time += elapsed_ms;
		stats.total_squared += elapsed_ms * elapsed_ms;
		stats.packets_received_timed++;
		if (!(flags & FLAG_FLOOD))
			printf(" time=%.3f ms\n", elapsed_ms);
	}
	

	return 0;
}

void print_stats(void) {
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

void signal_handler(int signum) {
	if (signum == SIGINT) {
		print_stats();
		exit(0);
	}
}

void	setup_signal_handlers(void) {
	struct sigaction sa = {0};
	sa.sa_handler = signal_handler;
	sigemptyset(&sa.sa_mask);
	sigaction(SIGINT, &sa, NULL);
}

int	main(int ac, char **arguments) {
	struct ping_opts options = {0};
	options.interval_ms = INTERVAL_DEFAULT;
	int		flags = get_flags(ac, arguments, &options);

	
	if (flags & FLAG_HELP) {
		show_help(arguments[0]);
		return 0;
	}

	
	if (optind >= ac) {
		fprintf(stderr, "%s: missing host operand\n", arguments[0]);
		return EX_USAGE;
	}
	
	if (flags & (FLAG_FLOOD | FLAG_INTERVAL)) {
		fprintf(stderr, "%s: ping: -f and -i incompatible options\n", arguments[0]);
		exit(1);
	}

	// Génération de la socket brute ICMP
	struct target_host host;
	host.host = arguments[optind];
	host.socket = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
	if (host.socket < 0) {
		perror("socket");
		return 1;
	}
	setsockopt(host.socket, IPPROTO_IP, IP_TTL, &options.ttl, sizeof(options.ttl));

	struct addrinfo hints = {0};
	struct addrinfo *res;
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_RAW;
	hints.ai_protocol = IPPROTO_ICMP;
	int gai = getaddrinfo(host.host, NULL, &hints, &res);
	if (gai != 0) {
		fprintf(stderr, "%s: unknown host\n", arguments[0]);
		return 1;
	}
	
	struct sockaddr_in *sin = (struct sockaddr_in *)res->ai_addr;
	
	// connection au host	
	host.addr = *sin;
	host.addr.sin_port = 0;

	freeaddrinfo(res);

	struct pollfd pfds[1] = {
		{ .fd = host.socket, .events = POLLIN }
	};
	
	struct timespec ts_last_sent = {0, 0};
	struct timespec ts_now;
	long unsigned elapsed_ms;

	host.icmp_sent.type = 8;
	host.icmp_sent.code = 0;
	host.icmp_sent.checksum = 0;
	host.icmp_sent.un.echo.id = 0;
	host.icmp_sent.un.echo.sequence = 0;

	printf("PING %s (%s): %d data bytes", host.host,
		inet_ntoa(host.addr.sin_addr), PAYLOAD_SIZE);
	if (flags & FLAG_VERBOSE)
		printf(", id 0x%x = %d\n", ntohs(htons(getpid())), ntohs(htons(getpid())));
	else
		printf("\n");

	setup_signal_handlers();

	while (!(flags & FLAG_COUNT) || options.count == 0 || stats.packets_sent < options.count) {
		clock_gettime(CLOCK_MONOTONIC, &ts_now);
		elapsed_ms = (ts_now.tv_sec - ts_last_sent.tv_sec) * 1000 + (ts_now.tv_nsec - ts_last_sent.tv_nsec) / 1000000;
		if (elapsed_ms >= options.interval_ms || ts_last_sent.tv_nsec == 0) {
			ts_last_sent = send_packet(stats.packets_sent, &host, flags & FLAG_SIZE ? options.size : PAYLOAD_SIZE);
			elapsed_ms = 0;
		}

		if (poll(pfds, 1, options.interval_ms - elapsed_ms) > 0)
			receive_packet(&host, flags, &options);
	}

	close(host.socket);
	print_stats();

	return (0);
}