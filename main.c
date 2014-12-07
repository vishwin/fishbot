/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>

#include <sys/socket.h>
#include <netdb.h>

#define MSG_LEN 512

int main() {
	// hints for socket creation, address resolution
	struct addrinfo hints;
	hints.ai_family=AF_INET;
	hints.ai_socktype=SOCK_STREAM;
	hints.ai_protocol=0;
	hints.ai_flags=0;
	
	// create socket
	int sock=socket(hints.ai_family, hints.ai_socktype, hints.ai_protocol);
	if (sock<0) {
		perror("socket");
		return 1;
	}
	
	// resolve address
	struct addrinfo *addr_list;
	int error=getaddrinfo("chat.freenode.net", "6667", &hints, &addr_list);
	if (error!=0) {
		fprintf(stderr, "%s\n", gai_strerror(error));
		return 1;
	}
	
	// connect
	struct addrinfo *addr_curr=addr_list;
	while (connect(sock, addr_curr->ai_addr, addr_curr->ai_addrlen)!=0) {
		if (addr_curr->ai_next==NULL) {
			perror("connect");
			freeaddrinfo(addr_list);
			return 1;
		}
		addr_curr=addr_curr->ai_next;
	}
	freeaddrinfo(addr_list);
	
	// identify ourself and join a channel
	char message[MSG_LEN+1]; // for null char
	char reply[MSG_LEN+1];
	snprintf(message, MSG_LEN, "NICK vshbot\r\n");
	write(sock, message, strlen(message));
	snprintf(message, MSG_LEN, "USER tsstbot 8 * :vishwin's bot\r\n");
	write(sock, message, strlen(message));
	snprintf(message, MSG_LEN, "JOIN ##vishwin\r\n");
	write(sock, message, strlen(message));
	
	while (read(sock, reply, MSG_LEN)!=0) {
		printf(reply);
		memset(reply, '\0', MSG_LEN+1);
	}
	
	// close connection
	snprintf(message, MSG_LEN, "QUIT :Test complete\r\n");
	write(sock, message, MSG_LEN);
	printf(message);
	close(sock);
	sock=-1;
	
	return 0;
}
