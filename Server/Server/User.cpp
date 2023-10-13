#include "User.h"

User::User()
{
}


User::~User()
{
}

void User::init(std::wstring UserName, std::wstring email, std::wstring password)
{
    if (!verifyUName(UserName))
        throw(std::invalid_argument("Invalid name!"));

    if (!verifyEmail(email))
        throw(std::invalid_argument("Invalid email!"));

    this->UserName = UserName;
    this->email = email;
    this->password = password;
}

std::wstring User::getUsername()
{
    return this->UserName;
}

std::wstring User::getEmail()
{
    return this->email;
}

std::wstring User::getPassword()
{
    return this->password;
}

bool User::verifyEmail(const std::wstring toVerify)
{
    std::wstring emailProvider, domain;

    bool dot = false, at = false;

    for (size_t i = 0; i < toVerify.size(); i++)
    {
        if (toVerify[i] == '@')
        {
            at = true;
            continue;
        }

        if (toVerify[i] == '.')
        {
            dot = true;
            continue;
        }

        if (at && !dot)
        {
            emailProvider.push_back(toVerify[i]);
        }
        else if (at && dot)
        {
            domain.push_back(toVerify[i]);
        }

    }

    if (emailProvider != L"gmail" && emailProvider != L"abv" && emailProvider != L"yahoo")
        return false;

    if (domain != L"com" && domain != L"bg")
        return false;

    return true;
}

bool User::verifyUName(const std::wstring toVerify)
{
    if (toVerify.size() >= 10)
        return false;

    for (size_t i = 0; i < toVerify.size(); i++)
    {
        switch (toVerify[i])
        {
        case '.':
        case '!':
        case '@':
        case '%':
        case '^':
        case '(':
        case ')':
        case '+':
        case '\\':
        case ';':
        case '/':
        case '?':
        case '~':
            return false;
            break;
        }
    }
    return true;
}
