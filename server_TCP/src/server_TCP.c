/*
 ============================================================================
 Name        : server_TCP.c
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
#include "math.h"

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
	int server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (server_socket < 0) {
		printf("socket creation failed.\n");
		clearwinsock();
		return -1;
	}

	// Set port and IP:
	struct sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	if (argc > 1) {
		server_addr.sin_port = htons(atoi(argv[1]));
	} else {
		server_addr.sin_port = htons(PROTOPORT);
	}

	// Bind to the set port and IP
	if (bind(server_socket, (struct sockaddr*) &server_addr,
			sizeof(server_addr)) < 0) {
		printf("bind() failed.\n");
		closesocket(server_socket);
		clearwinsock();
		return -1;
	}

	// Listen for clients:
	int qlen = 5;
	if (listen(server_socket, qlen) < 0) {
		printf("listen() failed.\n");
		closesocket(server_socket);
		return -1;
	}

	struct sockaddr_in client_addr;
	int client_socket;
	int client_len;

	//LOOP:
	//1-Accept an incoming connection
	//2-Send and Received data
	//3-Close connection
	while (1) {
		printf("Waiting for a client to connect...");
		//Accept an incoming connection
		client_len = sizeof(client_addr); // set the size of the client address
		if ((client_socket = accept(server_socket,
				(struct sockaddr*) &client_addr, &client_len)) < 0) {
			printf("accept() failed.\n");
			closesocket(client_socket);
			clearwinsock();
			return -1;
		}
		printf("Connection established %s: %d\n", inet_ntoa(client_addr.sin_addr),  client_addr.sin_port);

		while (1) {
			// Receive client's message:
			printf("Received:");
			if (recv(client_socket, client_message, sizeof(client_message), 0)
					< 0) {
				closesocket(client_socket);
				printf("Connection closed\n\n");
				break;
			}

			printf("%s\n", client_message);

			if(client_message[0]=='='){
				closesocket(client_socket);
				printf("Connection closed\n\n");
				break;
			}

			// Splitting client message into tokens"
			char *token;
			char *string_set[2];
			token = strtok(client_message, " ");
			int i = 0;
			while (token != NULL) {
				string_set[i] = token;
				i++;
				token = strtok(NULL, " ");
			}

			char operator = string_set[0][0];
			int operand1 = atoi(string_set[1]);//Convert string into integer
			int operand2 = atoi(string_set[2]);
			float result = 0;

			switch (operator) {
			case '+':
				result = add(operand1, operand2);
				break;
			case '*':
				result = mult(operand1, operand2);
				break;

			case '-':
				result = sub(operand1, operand2);
				break;

			case '/':
				result = division(operand1, operand2);
				break;

			}

			// Convert float into string
			gcvt(result, 6, server_message);

			if (send(client_socket, server_message, strlen(server_message), 0)
					< 0) {
				printf("Can't send\n");
				return -1;
			}

			// Clean buffers
			memset(server_message, '\0', sizeof(server_message));
			memset(client_message, '\0', sizeof(client_message));
		}
	}
	return 0;
} // main end
