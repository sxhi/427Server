#include <iostream>
#include <Winsock2.h>
#include <fstream>
#include <sstream>
#include <string>
#include <stdio.h>
#include <sys/types.h>
#include <vector>
#include <WS2tcpip.h>
#pragma comment (lib, "ws2_32.lib")
#define SERVER_PORT 8645
#define BUFFER 4096

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
	struct sockaddr_in sin;

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
	char buf[BUFFER];
	fstream file; // Output File
	ifstream save; // Inputing output file
	string first, second, third, fourth, total, space = " ";
	stringstream in, back;
	int idnum = 1;
	bool loop_control = true;

	while (loop_control) {
		ZeroMemory(buf, BUFFER);

		// Wait for client to send data
		int bytesReceived = recv(clientSocket, buf, BUFFER, 0);
		if (bytesReceived == SOCKET_ERROR) {
			cerr << "Error in recv()" << endl;
			break;
		}
		if (bytesReceived == 0) {
			cout << "Client Disconnected" << endl;
			break;
		}
		string clientIn = string(buf, 0, bytesReceived);
		in.str(clientIn);
		in >> first >> second >> third >> fourth;

			if (first == "add") {
				file.open("output.txt");
				file << idnum << space << second << space << third << space << fourth << endl;
				idnum++;
				send(clientSocket, "200 OK", 7, 0);
				file.close();
			}
			else if (first == "delete") {
			cout << "Shut up" << endl;
			send(clientSocket, "200 OK", 7, 0);
			}
			else if (first == "list") {
				string line;
				while (in >> first >> second >> third >> fourth) {
					total = second + space + third + space + fourth;

					save.open("output.txt");
					while (!save.eof()) {
						getline(save, line);
						cout << line << endl;
					}
					save.close();
				}
			}
			else if (first == "shutdown") {
				loop_control = false;
				return;
			}
			else {
				return;
			}
		cout << string(buf, 0, bytesReceived) << endl;
		// send(clientSocket, buf, bytesReceived + 1, 0);

	}
	// Close the socket
	closesocket(clientSocket);

	// Shutdown winsock
	WSACleanup();
}