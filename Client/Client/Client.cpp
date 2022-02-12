#include <iostream>
#include <string>
#include <WS2tcpip.h>
#pragma comment(lib, "WS2_32.lib")
#define SERVER_PORT 8645
#define BUFFER 4096

using namespace std;

void main() {

	string ipAddress = "127.0.0.1"; // Ip address of server
	int port = SERVER_PORT;

	// Initialize winsock
	WSADATA data;
	WORD ver = MAKEWORD(2, 2);
	int wsResult = WSAStartup(ver, &data);
	if (wsResult != 0) {
		cerr << "Cant start winsock! Error #" << wsResult << endl;
		return;
	}

	// Create Socket
	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET) {
		cerr << "Cannot create socket!" << endl;
		WSACleanup();
		return;
	}

	// Fill in hint structure
	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(SERVER_PORT);
	inet_pton(AF_INET, ipAddress.c_str(), &hint.sin_addr);

	// Connect to server
	int connection = connect(sock, (sockaddr*)&hint, sizeof(hint));
	if (connection == SOCKET_ERROR) {
		cerr << "Cant connect to server!" << endl;
		closesocket(sock);
		WSACleanup();
		return;
	}

	// Do While loop to send and recieve data
	char buf[BUFFER];
	string userInput;
	cout << "List of Commands:" << endl;
	cout << "#1: Type 'add' followed by 'first name' 'last name' and 'phone number' to add an employee. " << endl;
	cout << "#2: Type 'delete' followed by an 'id' to delete an employee." << endl;
	cout << "#3: Type 'list' to list all employees." << endl;
	cout << "#4: Type 'shutdown' to shut down server. " << endl;
	cout << "#5: Type 'quit' to disconnect from server. " << endl;

	do {
		// Prompt user for text
		cout << "> ";
		getline(cin, userInput);

		if (userInput.size() > 0) {  // Make sure user typed something in
			// Send text
			int sendResult = send(sock, userInput.c_str(), userInput.size() + 1, 0);
			if (sendResult != SOCKET_ERROR) {

				// Wait for response
				ZeroMemory(buf, BUFFER);
				int bytesReceived = recv(sock, buf, BUFFER, 0);

				if (bytesReceived > 0) {
					// Echo response to console
					cout << "SERVER> " << string(buf, 0, bytesReceived) << endl;
				}
				if (bytesReceived > 0) {
					// Echo response to console
					cout << "SERVER> " << string(buf, 0, bytesReceived) << endl;
				}
			}
		}

	} while (userInput.size() > 0);

	// Close down client
	closesocket(sock);
	WSACleanup();
}