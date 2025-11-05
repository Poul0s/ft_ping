/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: psalame <psalame@student.42angouleme.fr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/01 15:10:14 by psalame           #+#    #+#             */
/*   Updated: 2025/11/05 16:41:36 by psalame          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ping.h"
#include <sysexits.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <netinet/ip_icmp.h>

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

int	main(int ac, char **arguments) {
	int		flags = get_flags(ac, arguments);

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
	// (todo) require root to make raw socket. Does subject allow this?
	host.socket = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
	if (host.socket < 0) {
		perror("socket");
		return 1;
	}

	// connection au host
	host.addr.sin_family = AF_INET;
	host.addr.sin_port = 0;
	if (inet_pton(AF_INET, host.host, &host.addr.sin_addr) <= 0) {
		fprintf(stderr, "%s: unknown host\n", arguments[0]);
		return 1;
	}


	// Préparation du paquet ICMP ECHO REQUEST
	struct icmphdr icmp;
	icmp.type = 8; // ICMP_ECHO
	icmp.code = 0;
	icmp.checksum = 0;
	icmp.un.echo.id = htons(getpid()); // todo how does inetutils generate his ID?
	icmp.un.echo.sequence = htons(1);

	char payload[56] = "Hello, this is a ping payload!"; // todo how does inetutils generate his payload?
	char packet[sizeof(icmp) + sizeof(payload)];
	memcpy(packet, &icmp, sizeof(icmp));
	memcpy(packet + sizeof(icmp), payload, sizeof(payload));
	((struct icmphdr *)packet)->checksum = checksum16(packet, sizeof(packet));
	
	// Envoi du paquet
	ssize_t bytes_sent = sendto(host.socket, packet, sizeof(packet), 0,
		(struct sockaddr *)&host.addr, sizeof(host.addr));
	if (bytes_sent < 0) {
		perror("sendto");
		return 1;
	}


	// reception du paquet
	char recv_buffer[1024];
	struct sockaddr_in recv_addr;
	socklen_t addr_len = sizeof(recv_addr);
	ssize_t bytes_received = recvfrom(host.socket, recv_buffer, sizeof(recv_buffer), 0,
		(struct sockaddr *)&recv_addr, &addr_len);
	if (bytes_received < 0) {
		perror("recvfrom");
		return 1;
	}
	struct iphdr *ip_header = (struct iphdr *)recv_buffer;
	size_t ip_header_len = ip_header->ihl * 4;
	struct icmphdr *recv_icmp = (struct icmphdr *)(recv_buffer + ip_header_len);
	printf("%zd bytes from %s: type=%u code=%u id=%u icmp_seq=%d ttl=%d\n",
		bytes_received - ip_header_len, // must be sizeof(icmp) + sizeof(payload) : good
		inet_ntoa(recv_addr.sin_addr),
		recv_icmp->type, // must be 8 for echo reply : good
		recv_icmp->code, // must be 0 for reply : good
		ntohs(recv_icmp->un.echo.id), // must be pid : good
		ntohs(recv_icmp->un.echo.sequence), // must be 1 : good
		ip_header->ttl);

	// fermeture de la socket
	close(host.socket);

	printf(" pid %d\n", getpid());

	return (0);
}