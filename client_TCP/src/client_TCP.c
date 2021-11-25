/*
 ============================================================================
 Name        : client_TCP.c
 Author      : Daniele Cecca & Michele Fraccalvieri
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */
#if defined WIN32
#include <winsock.h>
#else
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#define closesocket close
#endif

#include <stdio.h>
#include <string.h>
#include "prot_appl.h"

#define NO_ERROR 0
void clearwinsock() {
#if defined WIN32
	WSACleanup();
#endif
}

int main(int argc, char *argv[]) {
	setvbuf(stdout, NULL, _IONBF, 0);

#if defined WIN32
	// Initialize Winsock
	WSADATA wsa_data;
	int result = WSAStartup(MAKEWORD(2, 2), &wsa_data);
	if (result != NO_ERROR) {
		printf("Error at WSAStartup()\n");
		return 0;
	}
#endif

	char server_message[BUFF], client_message[BUFF];

	// Clean buffers
	memset(server_message, '\0', sizeof(server_message));
	memset(client_message, '\0', sizeof(client_message));

	//Create a socket
	int client_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (client_socket < 0) {
		printf("socket creation failed.\n");
		clearwinsock();
		return -1;
	}

	// Set port and IP the same as server-side
	struct sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;
	if (argc > 1) {
		server_addr.sin_addr.s_addr = inet_addr(argv[1]);
		server_addr.sin_port = htons(atoi(argv[2]));
	} else {
		server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
		server_addr.sin_port = htons(PROTOPORT);
	}

	// Send connection request to server
	if (connect(client_socket, (struct sockaddr*) &server_addr,
			sizeof(server_addr)) < 0) {
		printf("Failed to connect.\n");
		clearwinsock();
		return 0;
	}

	while (1) {

		/* To do control on input I have to use a regular expression but at
		 * the end of this exercise, I decided to not do it. Also because
		 * it is not specified on track of the exercise.
		 * So use only the correct pattern for inputs
		 */

		// Get input from the user

		printf(	"Insert an operation with this pattern: [operator] [num] [num] (ex. + 2 4).\n"
				"Valid operators are: + - * /.\n"
				"To close connection insert: =.\n"
				"Input:");
		gets(client_message);

		// Send the message to server
		if (send(client_socket, client_message, strlen(client_message), 0)
				< 0) {
			printf("Unable to send message\n");
			closesocket(client_socket);
			clearwinsock();
			return -1;
		}

		if (client_message[0] == '=') {
			break;
		}

		printf("Received: ");

		if (recv(client_socket, server_message, sizeof(server_message), 0)
				<= 0) {
			printf("Error while receiving server's msg\n");
			closesocket(client_socket);
			clearwinsock();
			return -1;
		}

		printf("%s\n\n", server_message);

		// Clean buffers
		memset(server_message, '\0', sizeof(server_message));
		memset(client_message, '\0', sizeof(client_message));

	}

	//Close socket
	closesocket(client_socket);
	clearwinsock();
	printf("\n"); // Print a final linefeed
	system("pause");
	return (0);

} //end main
