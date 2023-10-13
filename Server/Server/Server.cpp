#include<iostream>
#include<string>
#include<winsock2.h>
#include<ws2tcpip.h>
#include <tchar.h>
#include <omp.h>
#include "User.h"
#include <sqltypes.h>
#include <sql.h>
#include <sqlext.h>
#include <curl/curl.h>

using std::cout;
using std::endl;
using std::cerr;

void sendEmail(std::wstring email)
{
	CURL* curl;
	CURLcode res = CURLE_OK;

	curl = curl_easy_init();
	if (curl) {
		curl_easy_setopt(curl, CURLOPT_URL, "smtps://smtp.gmail.com:465");

		curl_easy_setopt(curl, CURLOPT_USERNAME, "martokonov@gmail.com");
		curl_easy_setopt(curl, CURLOPT_PASSWORD, "opke exyf bwof nhqf");


		curl_easy_setopt(curl, CURLOPT_MAIL_FROM, "martokonov@gmail.com");
		curl_easy_setopt(curl, CURLOPT_MAIL_RCPT, email.c_str());

		const char* email_data = "Subject: Your Registration Confirmation\n From: martokonov@gmail.com\n\n Thank you for registering!";

		curl_easy_setopt(curl, CURLOPT_READDATA, email_data);

		// Send the email
		res = curl_easy_perform(curl);

		if (res != CURLE_OK) {
			fprintf(stderr, "Email sending failed: %s\n", curl_easy_strerror(res));
		}

		curl_easy_cleanup(curl);
	}

}


bool loginSuccess(SOCKET& acptSock, SQLHDBC& hdbc, wchar_t usernameBuffer[100], wchar_t passwordBuffer[100])
{
	SQLWCHAR queryUsername[] = L"SELECT UserName FROM Users";
	SQLWCHAR queryPassword[] = L"SELECT Password FROM Users";

	if (SQLExecDirectW(hdbc, queryUsername, SQL_NTS) != SQL_SUCCESS || SQLExecDirectW(hdbc, queryPassword, SQL_NTS) != SQL_SUCCESS)
		throw std::exception("SQLExecDirectW with queryUsername or queryPassword failed");


	SQLWCHAR dataUsername[100];
	SQLWCHAR dataPassword[100];
	SQLLEN userLen, pasLen;

	while (SQLFetch(hdbc) == SQL_SUCCESS)
	{
		SQLGetData(hdbc, 1, SQL_WCHAR, dataUsername, sizeof(dataUsername), &userLen);
		SQLGetData(hdbc, 1, SQL_WCHAR, dataPassword, sizeof(dataPassword), &pasLen);

		if ((std::wcscmp(usernameBuffer, dataUsername) == 0)
			&& std::wcscmp(passwordBuffer, dataPassword) == 0)
				return true;
	}

	return false;
}

bool registrationSuccess(SOCKET& acptSock, SQLHDBC& hdbc, User* user)
{
	SQLWCHAR queryUsername[] = L"SELECT UserName FROM Users";
	if (SQLExecDirectW(hdbc, queryUsername, SQL_NTS) != SQL_SUCCESS)
		throw std::exception("SQLExecDirectW with queryUsername or failed");

	SQLWCHAR dataUsername[100];
	SQLLEN userLen;
	while (SQLFetch(hdbc) == SQL_SUCCESS)
	{
		SQLGetData(hdbc, 1, SQL_WCHAR, dataUsername, sizeof(dataUsername), &userLen);

		if ((wcscmp(user->getUsername().c_str(), dataUsername) == 0))
		{
			cout << "Username taken" << endl;
			throw std::exception("Username taken");
		}
	}

	SQLWCHAR insertStatement[] = L"INSERT INTO Users (UserName, Password, Email) VALUES (?, ?, ?)";
	SQLHSTMT hstmt;
	SQLLEN usernameLen = SQL_NTS;
	SQLLEN passwordLen = SQL_NTS;
	SQLLEN emailLen = SQL_NTS;

	SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);

	SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR, 0, 0, (SQLPOINTER)user->getUsername().c_str(), 0, &usernameLen);
	SQLBindParameter(hstmt, 2, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR, 0, 0, (SQLPOINTER)user->getPassword().c_str(), 0, &passwordLen);
	SQLBindParameter(hstmt, 3, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR, 0, 0, (SQLPOINTER)user->getEmail().c_str(), 0, &emailLen);

	if (SQLExecDirectW(hstmt, insertStatement, SQL_NTS) == SQL_SUCCESS) {
		cout << "User registration successful" << endl;
		sendEmail(user->getEmail());
		return true;
	}
	else {
		cerr << "User registration failed" << endl;
		return false;
	}

}

wchar_t* charToWChar(const char* input) {
	size_t size = strlen(input) + 1;
	wchar_t* output = new wchar_t[size];

	size_t outSize;
	mbstowcs_s(&outSize, output, size, input, size - 1);

	return output;
}


int main()
{

	SQLHENV henv;
	SQLHDBC hdbc;
	SQLRETURN retcode;

	retcode = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &henv);
	retcode = SQLSetEnvAttr(henv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, 0);

	retcode = SQLAllocHandle(SQL_HANDLE_DBC, henv, &hdbc);

	SQLWCHAR connectionStr[] = L"DRIVER={SQL Server};SERVER=DESKTOP-Q9LONS8\\SQLEXPRESS02,1433;DATABASE=RegistrationBse;";
	SQLSMALLINT connStrLen = SQL_NTS; // Use SQL_NTS to indicate the null-terminated string
	retcode = SQLConnect(hdbc, connectionStr, connStrLen, NULL, 0, NULL, 0);

	SQLWCHAR szSqlState[6];
	SQLINTEGER pfNativeError;
	SQLWCHAR szErrorMsg[SQL_MAX_MESSAGE_LENGTH];
	SQLSMALLINT cbErrorMsgMax;
	SQLSMALLINT cbErrorMsg;
	if (retcode != SQL_SUCCESS) {
		std::cerr << "Database connection failed." << std::endl;
		SQLGetDiagRec(SQL_HANDLE_DBC, hdbc, 1, szSqlState, &pfNativeError, szErrorMsg, SQL_MAX_MESSAGE_LENGTH, &cbErrorMsg);
		wprintf(L"SQLConnect failed with error %d: %ls\n", pfNativeError, szErrorMsg);
		SQLFreeHandle(SQL_HANDLE_DBC, hdbc);
		SQLFreeHandle(SQL_HANDLE_ENV, henv);
	}

	cout << "\t\tSERVER" << endl << endl;

	int port = 55555;

	WSADATA wsa;
	WORD versReq = MAKEWORD(2, 2);


	if (WSAStartup(versReq, &wsa) != 0)
	{
		cerr << "DLL not found" << endl;
		return 0;
	}

	SOCKET servSock, acptSock;
	servSock = INVALID_SOCKET;

	servSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (servSock == INVALID_SOCKET)
	{
		cerr << "invalid socket" << WSAGetLastError() << endl;

		WSACleanup();
		return 0;
	}

	sockaddr_in servAddr;
	servAddr.sin_family = AF_INET;

	InetPton(AF_INET, _T("127.0.0.1"), &servAddr.sin_addr.s_addr);

	servAddr.sin_port = htons(port);

	if (bind(servSock, (SOCKADDR*)&servAddr, sizeof(servAddr)) == SOCKET_ERROR)
	{
		cout << "bind failed: " << WSAGetLastError() << endl;
		closesocket(servSock);
		WSACleanup;
		return 0;
	}


	if (listen(servSock, 1) == SOCKET_ERROR)
	{
		cerr << "listen failed" << WSAGetLastError() << endl;
	}


	bool terminateWhile = false;
	bool login;

	acptSock = accept(servSock, NULL, NULL);
	if (acptSock == INVALID_SOCKET)
	{
		cerr << "accept failed" << WSAGetLastError() << endl;
		WSACleanup();
		return 0;
	}
		
	//--
	if (recv(acptSock, (char*)&login, sizeof(bool), 0) <= 0)
		WSACleanup();


	char usernameBuffer[100], emailBuffer[100], passwordBuffer[100];
	int bytesRecv;
	int userSize, passSize;
	//-----
	if (recv(acptSock, (char*)(&userSize), sizeof(userSize), 0) <= 0)
		WSACleanup();

	bytesRecv = recv(acptSock, usernameBuffer, userSize, 0);
	if (bytesRecv <= 0)
		WSACleanup();
	usernameBuffer[bytesRecv] = '\0';

	//---------

	if (!login)
	{

		int mailSize;
		
		if(recv(acptSock, reinterpret_cast<char*>(&mailSize), sizeof(mailSize), 0) <= 0)
			WSACleanup();

		bytesRecv = recv(acptSock, emailBuffer, mailSize, 0);
		if (bytesRecv <= 0)
			WSACleanup();
		emailBuffer[bytesRecv] = '\0';
	}

	//---------
	if (recv(acptSock, (char*)(&passSize), sizeof(passSize), 0) <= 0)
		WSACleanup();

	bytesRecv = recv(acptSock, passwordBuffer, passSize, 0);
	if (bytesRecv <= 0)
		WSACleanup();
	passwordBuffer[bytesRecv] = '\0';

	bool success;
	if (login)
	{

		wchar_t* usernameWbuffer = charToWChar(usernameBuffer);
		wchar_t* passwordWBuffer = charToWChar(passwordBuffer);
		try
		{
			success = loginSuccess(acptSock, hdbc, usernameWbuffer, passwordWBuffer);
		}
		catch (std::exception& e)
		{
			cout << e.what() << endl;
				
			if (send(acptSock, (char*) false, sizeof(bool), 0) <= 0)
				cerr << "error executing a query on login" << endl;
		}
		if (send(acptSock, reinterpret_cast<const char*>(&success), sizeof(bool), 0) <= 0)
			cerr << "error executing a query on login" << endl;


		if (success)
		{
			if (send(acptSock, reinterpret_cast<const char*>(&success), sizeof(bool), 0) <= 0)
				cerr << "error sending confirmation of login success" << endl;
		}
		else {
			bool toSend = false;
			if (send(acptSock, reinterpret_cast<const char*>(&toSend), sizeof(bool), 0) <= 0)
				cerr << "error sending denial of login success" << endl;
		}

	}
	else {


		User user;
		bool validation = true;

		wchar_t* usernameWbuffer = charToWChar(usernameBuffer);
		wchar_t* passwordWBuffer = charToWChar(passwordBuffer);
		wchar_t* emailWBuffer = charToWChar(emailBuffer);
		try
		{
			user.init(std::wstring(usernameWbuffer), std::wstring(passwordWBuffer), std::wstring(emailWBuffer));
		}
		catch (std::invalid_argument& e)
		{
			cerr << e.what() << endl;
			validation = false;
			if (send(acptSock, reinterpret_cast<const char*>(&validation), sizeof(bool), 0) <= 0)
				cerr << "error sending invalid argument to client" << endl;
				
		}
		if (send(acptSock, reinterpret_cast<const char*>(&validation), sizeof(bool), 0) <= 0)
			cerr << "error sending validation successful to client" << endl;

		try
		{
			success = registrationSuccess(acptSock, hdbc, &user);
		}
		catch (std::exception(&e))
		{
			cerr << e.what() << endl;
			success = false;
			if (send(acptSock, reinterpret_cast<const char*>(&success), sizeof(bool), 0) <= 0)
				cerr << "error sending exception to client" << endl;
		}
		if (send(acptSock, reinterpret_cast<const char*>(&success), sizeof(bool), 0) <= 0)
			cerr << "error sending no exception client" << endl;


		if (success)
		{
			sendEmail(std::wstring(emailWBuffer));
			if (send(acptSock, reinterpret_cast<const char*>(&success), sizeof(bool), 0) <= 0)
				cerr << "error sending confirmation of registration success" << endl;
		}
		else
		{
			if (send(acptSock, reinterpret_cast<const char*>(&success), sizeof(bool), 0) <= 0)
				cerr << "error sending denial of registration success" << endl;
		}

	}


	
	system("pause");
	WSACleanup();
	closesocket(acptSock);
	closesocket(servSock);
}