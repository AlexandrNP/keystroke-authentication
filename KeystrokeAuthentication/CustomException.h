#pragma once

#include <exception>
#include <string>

class CustomException: public std::exception
{
public:
	explicit CustomException(const std::wstring what_arg);
	explicit CustomException(const wchar_t* what_arg);
	std::wstring getReason();
private:
	std::wstring m_reason;
};