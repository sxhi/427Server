#include <iostream>
#include <Winsock2.h>
#include <fstream>
#include <sstream>
#include <string>
#include <cstring>
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

	// Variable Declarations

	char buf[BUFFER]; //Buffer / Message
	fstream file; // Output File
	ofstream temp; // Temp output file
	ifstream save; // Inputing output file
	int idnum = 1000; // Id#
	bool loop_control = true; // Loop Controller

	// While loop: accept and echo message back to client / Functions
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

		string first, second, third, fourth, fifth, total, fixed, newSecond, space = " "; // Each Word
		stringstream in, out; // Streaming String
		string clientIn = string(buf, 0, bytesReceived);
		in.str(clientIn);
		in >> first >> second >> third >> fourth >> fifth;

		// Add Function
			if (first == "add") {
				idnum++;
				file.open("output.txt", std::ios_base::app);
				file << idnum << space << second << space << third << space << fourth << space << fifth << endl;
				cout << "The new Record ID is " << idnum << endl;
				send(clientSocket, "200 OK", 7, 0);
				file.close();
			}

		// Delete Function
			else if (first == "delete") {
				int idcompare = stoi(second); // Create integer out of string
				
				if (idcompare > 1000 || idcompare < 0) {
					cout << "403 ID Invalid" << endl;
					send(clientSocket, "403 ID Invalid", 14, 0);
					}

				temp.open("temp.txt"); // Open a temp file
				file.open("output.txt"); // Open the writing file
				while (getline(file, fixed)) {
					out.str(fixed);
					out >> newSecond;
					if (newSecond != second) {
						temp << fixed << endl;
					}
				}
				file.close();
				temp.close();
				remove("output.txt");
				rename("temp.txt", "output.txt");
				cout << "200 OK " << endl;
				send(clientSocket, "200 OK", 7, 0);
			}

		// List Function
			else if (first == "list") {
				string line;
				file.open("output.txt");
				getline(file, line);
			
				if (!line.empty()) {
					string one, two, three, four, total;
					cout << "200 OK!" << endl;
					send(clientSocket, "200 OK", 7, 0);
					file.close();
					file.open("output.txt");


					while (file >> one >> two >> three >> four) {
						total = one + space + two + space + three + space + four + "!";
						cout << total << endl;
						strcpy_s(buf, total.c_str());
						send(clientSocket, buf, 50, 0);
					}		
					file.close();
				}
				else {
					cout << "The list is empty." << endl;
					send(clientSocket, "The list is empty!", 19, 0);
				}
			}

		// Shutdown Function
			else if (first == "shutdown") {
				loop_control = false;
				return;
			}
		// Quit Function
			else if (first == "quit") {
				loop_control = false;
				return;
			}
			else {
				return;
			}

	}

	// Close the socket
	closesocket(clientSocket);

	// Shutdown winsock
	WSACleanup();
}