/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: psalame <psalame@student.42angouleme.fr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/01 15:10:14 by psalame           #+#    #+#             */
/*   Updated: 2025/11/06 00:06:49 by psalame          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ping.h"
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <netinet/ip_icmp.h>
#include <time.h>
#include <linux/time.h>
#include <poll.h>
#include <netdb.h>
#include <signal.h>

u_int16_t packet_sent = 0;
u_int16_t packet_received = 0;

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

struct timespec send_packet(u_int16_t sequence, struct target_host *host) {
	const pid_t pid = getpid();
	static struct icmphdr icmp = { 
		.type = 8, 
		.code = 0, 
		.checksum = 0, 
		.un.echo = {0}
	}; // ICMP_ECHO
	
	char payload[PAYLOAD_SIZE];
	char packet[sizeof(icmp) + sizeof(payload)];
	
	// Remplissage des champs variables
	icmp.un.echo.id = htons(pid); // todo how does inetutils generate his ID?
	icmp.un.echo.sequence = htons(sequence);
	memcpy(packet, &icmp, sizeof(icmp));

	struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);
	snprintf(payload, sizeof(payload), "%ld.%09ld", ts.tv_sec, ts.tv_nsec); // theoric max possible size : 20 + 1 + 9 = 30
	memcpy(packet + sizeof(icmp), payload, sizeof(payload));
	
	((struct icmphdr *)packet)->checksum = checksum16(packet, sizeof(packet));

	
	// Envoi du paquet
	packet_sent++;
	ssize_t bytes_sent = sendto(host->socket, packet, sizeof(packet), 0,
		(struct sockaddr *)&host->addr, sizeof(host->addr));
	if (bytes_sent < 0)
		perror("sendto"); // debug only, todo remove later

	return ts;
}

int receive_packet(struct target_host *host, int flags, struct ping_opts *opts) {
	const pid_t pid = getpid();
	char recv_buffer[1024];
	struct sockaddr_in recv_addr;
	socklen_t addr_len = sizeof(recv_addr);
	ssize_t bytes_received = recvfrom(host->socket, recv_buffer, sizeof(recv_buffer), 0,
		(struct sockaddr *)&recv_addr, &addr_len);
		
	if (bytes_received < 0) {
		// todo test with ttl 0 if trigger this error
		// todo show stats
		perror("recvfrom");
		return 1;
	}

	
	struct timespec ts_recv;
	clock_gettime(CLOCK_MONOTONIC, &ts_recv);
	struct iphdr *ip_header = (struct iphdr *)recv_buffer;
	size_t ip_header_len = ip_header->ihl * 4;
	struct icmphdr *recv_icmp = (struct icmphdr *)(recv_buffer + ip_header_len);

	if (recv_icmp->type == 11) {
		printf("%zd bytes from %s: Time to live exceeded\n",
			bytes_received - ip_header_len,
			inet_ntoa(recv_addr.sin_addr)
		);
		return 0;
	}

	if (recv_icmp->un.echo.id != htons(pid) || recv_icmp->type != 0) {
		// not for us, ignore
		return 0;
	}
	packet_received++;

	printf("%zd bytes from %s: icmp_seq=%d ttl=%d",
		bytes_received - ip_header_len,
		inet_ntoa(recv_addr.sin_addr),
		ntohs(recv_icmp->un.echo.sequence),
		(flags & FLAG_TTL) ? opts->ttl - (64 - ip_header->ttl) : ip_header->ttl // todo better calculation of ttl
	);
	
	char *recv_payload = recv_buffer + ip_header_len + sizeof(struct icmphdr);
	recv_payload[bytes_received - ip_header_len - sizeof(struct icmphdr)] = '\0';
	char *separator = strchr(recv_payload, '.');
	if (!separator) {
		printf("\n");
	} else {
		*separator = '\0';
		long sec_sent = atol(recv_payload);
		long nsec_sent = atol(separator + 1);
		float elapsed_ms = (ts_recv.tv_sec - sec_sent) * 1000 + ((float)ts_recv.tv_nsec - nsec_sent) / 1000000;
		
		printf(" time=%.3f ms\n", elapsed_ms);
	}
	

	return 0;
}

void print_stats(void) {
	printf("--- Ping statistics ---\n");
	printf("%d packets transmitted, %d packets received, %d%% packet loss\n",
		packet_sent,
		packet_received,
		(int)(((float)(packet_sent - packet_received) / packet_sent) * 100)
	);
	// todo min/avg/max/stddev
}

void signal_handler(int signum) {
	if (signum == SIGINT) {
		print_stats();
		exit(0);
	}
}

void	setup_signal_handlers(void) {
	struct sigaction sa;
	sa.sa_handler = signal_handler;
	sigemptyset(&sa.sa_mask);
	sigaction(SIGINT, &sa, NULL);
}

int	main(int ac, char **arguments) {
	struct ping_opts options = {0};
	int		flags = get_flags(ac, arguments, &options);

	if (flags & FLAG_HELP) {
		show_help(arguments[0]);
		return 0;
	}

	if (optind >= ac) {
		fprintf(stderr, "%s: missing host operand\n", arguments[0]);
		return EX_USAGE;
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

	// todo getaddrinfo pour résoudre le nom de domaine en IP
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
	long elapsed_ms;

	printf("PING %s (%s): %d data bytes\n", host.host,
		inet_ntoa(host.addr.sin_addr), PAYLOAD_SIZE);

	setup_signal_handlers();

	while (true) {
		clock_gettime(CLOCK_MONOTONIC, &ts_now);
		elapsed_ms = (ts_now.tv_sec - ts_last_sent.tv_sec) * 1000 + (ts_now.tv_nsec - ts_last_sent.tv_nsec) / 1000000;
		if (elapsed_ms >= INTERVAL_DEFAULT) {
			ts_last_sent = send_packet(packet_sent, &host);
			elapsed_ms = 0;
		}

		if (poll(pfds, 1, INTERVAL_DEFAULT - elapsed_ms) > 0) {
			// recept packet
			int ret = receive_packet(&host, flags, &options);
			if (ret)
				return ret;
		}
	}

	// fermeture de la socket
	close(host.socket);

	return (0);
}