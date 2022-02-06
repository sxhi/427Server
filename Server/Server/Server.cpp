#include <iostream>
#include <WS2tcpip.h>
#pragma comment (lib, "ws2_32.lib")
#define SERVER_PORT 54000
using namespace std;

void main()
{
	// Initilize winsock
	WSADATA WsData;
	WORD ver = MAKEWORD(2, 2);
	int wsOk = WSAStartup(ver, &WsData);

	if (wsOk != 0) {
		cerr << "Cant initialize winsock!" << endl;
		return;
	}

	// Create a socket
	SOCKET listening = socket(AF_INET, SOCK_STREAM, 0);
	if (listening == INVALID_SOCKET) {
		cerr << "Cant create a socket!" << endl;
		return;
	}
	// Bind socket to ip and port
	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(SERVER_PORT);
	hint.sin_addr.S_un.S_addr = INADDR_ANY;
	bind(listening, (sockaddr*)&hint, sizeof(hint));

	// Tell winsock the socket is for listening
	listen(listening, SOMAXCONN);

	// Wait for a connection
	sockaddr_in client;
	int clientSize = sizeof(client);

	SOCKET clientSocket = accept(listening, (sockaddr*)&client, &clientSize);
	if (clientSocket == INVALID_SOCKET) {
		cerr << "Cannot bind client socket!" << endl;
		return;
	}

	char host[NI_MAXHOST]; // Clients remote name
	char service[NI_MAXHOST]; // Service (i.e. port) the client is connect on

	ZeroMemory(host, NI_MAXHOST); // For Mac
	ZeroMemory(service, NI_MAXHOST); // For Mac

	if (getnameinfo((sockaddr*)&client, sizeof(client), host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0) {
		cout << host << " successfully connected on port " << service << endl;
	}
	else {
		inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
		cout << host << "connected on port" << ntohs(client.sin_port) << endl;
	}

	// Close listening socket
	closesocket(listening);

	// While loop: accept and echo message back to client
	char buf[4096];

	while (true) {
		ZeroMemory(buf, 4096);

		// Wait for client to send data
		int bytesReceived = recv(clientSocket, buf, 4096, 0);
		if (bytesReceived == SOCKET_ERROR) {
			cerr << "Error in recv()" << endl;
			break;
		}
		if (bytesReceived == 0) {
			cout << "Client Disconnected" << endl;
			break;
		}
		cout << string(buf, 0, bytesReceived) << endl;
		send(clientSocket, buf, bytesReceived + 1, 0);
	}
	// Close the socket
	closesocket(clientSocket);

	// Shutdown winsock
	WSACleanup();
}