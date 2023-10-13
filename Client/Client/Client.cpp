#include<iostream>
#include<winsock2.h>
#include<ws2tcpip.h>
#include <tchar.h>
#include<vector>
#include "UIHandle.h"

using namespace std;



int WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nShowCmd)
{
	UIHandle ui;

	ui.init();

	bool shouldProceed = ui.run();
	if (!shouldProceed)
		return 0;


	bool login = ui.loginWindow;

	cout << "\t\tCLIENT" << endl << endl;


	SOCKET clientSock;
	int port = 55555;
	WSADATA wsa;
	WORD versReq = MAKEWORD(2, 2);
	if (WSAStartup(versReq, &wsa) != 0)
	{
		cout << "DLL failed" << endl;
		return 0;
	}

	clientSock = INVALID_SOCKET;
	clientSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (clientSock == INVALID_SOCKET)
	{
		cout << "error setting up socket" << WSAGetLastError() << endl;
		WSACleanup();
		return 0;
	}

	sockaddr_in clientAddr;
	clientAddr.sin_family = AF_INET;
	InetPton(AF_INET, _T("127.0.0.1"), &clientAddr.sin_addr.s_addr);
	clientAddr.sin_port = htons(port);

	//clientSock- passes the client socket
	// clientAddr structure conatining server IP address and port
	if (connect(clientSock, (SOCKADDR*)&clientAddr, sizeof(clientAddr)) == SOCKET_ERROR) {
		cout << "connection to server failed" << endl;
		WSACleanup();
		return 0;
	}

	if (send(clientSock, (char*)&login, sizeof(bool), 0) <= 0)
		WSACleanup();

	//username
	const char* user = ui.information[0].c_str();
	int size = ui.information[0].size();

	// First, send the size of the data
	if (send(clientSock, reinterpret_cast<const char*>(&size), sizeof(size), 0) <= 0)
		WSACleanup();

	// Then, send the actual data
	if (send(clientSock, user, size, 0) <= 0)
		WSACleanup();


	//email
	if (!login)
	{
		const char* email = ui.information[2].c_str();
		int newsize = ui.information[2].size();

		// First, send the size of the data
		if (send(clientSock, reinterpret_cast<const char*>(&newsize), sizeof(newsize), 0) <= 0)
			WSACleanup();

		// Then, send the actual data
		if (send(clientSock, email, newsize, 0) <= 0)
			WSACleanup();

	}


	//password
	const char* pass = ui.information[1].c_str();
	size = ui.information[1].size();

	if (send(clientSock, reinterpret_cast<const char*>(&size), sizeof(size), 0) <= 0)
		WSACleanup();


	if (send(clientSock, pass, sizeof(pass), 0) <= 0)
		WSACleanup();

	if (login)
	{
		bool querysuccess;
		if (recv(clientSock, reinterpret_cast<char*>(&querysuccess) ,sizeof(bool), 0) <= 0)
			WSACleanup();
		
		ui.init(300,300);
		if (!querysuccess)
		{
			ui.result("Error query");
		}

		bool loginsuccess;
		if (recv(clientSock, reinterpret_cast<char*>(&loginsuccess), sizeof(bool), 0) <= 0)
			WSACleanup();

		switch (loginsuccess)
		{
		case true:
			ui.result("Login Success");
		case false:
			ui.result("Login failure");
		}

	}
	else
	{

		bool validationSuccessful;
		if (recv(clientSock, reinterpret_cast<char*>(&validationSuccessful), sizeof(bool), 0) <= 0)
			WSACleanup();

		ui.init(400,400);
		if (!validationSuccessful)
		{
			ui.result("Invalid username\n,password\nor Email");
			system("pause");
			WSACleanup();
			closesocket(clientSock);
			return 0; 
		}
		
		bool querySuccess;
		if (recv(clientSock, reinterpret_cast<char*>(&querySuccess), sizeof(bool), 0) <= 0)
			WSACleanup();

		if (!querySuccess)
		{
			ui.result("Error with\nquery to\ndata server");
			system("pause");
			WSACleanup();
			closesocket(clientSock);
			return 0;
		}

		bool registrationSuccess;
		if (recv(clientSock, reinterpret_cast<char*>(&registrationSuccess), sizeof(bool), 0) <= 0)
			WSACleanup();

		if (!registrationSuccess)
		{
			ui.result("Unsuccessful\nregistration!");
			system("pause");
			WSACleanup();
			closesocket(clientSock);
			return 0;
		}
		
		ui.result("Successful\nregistration!");
	}


	system("pause");
	WSACleanup();
	closesocket(clientSock);

}