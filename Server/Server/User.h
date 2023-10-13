#pragma once
#include <string>
#include <stdexcept>


class User
{
	std::wstring UserName;
	std::wstring email;
	std::wstring password;

public:
	User();
	~User();
	void init(std::wstring UserName, std::wstring email, std::wstring password);
	std::wstring getUsername();
	std::wstring getEmail();
	std::wstring getPassword();

private:

	bool verifyEmail(std::wstring toVerify);
	bool verifyUName(std::wstring toVerify);

};

