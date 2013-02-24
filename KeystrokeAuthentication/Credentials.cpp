#include "stdafx.h"
#include "Credentials.h"

bool Credentials::isPasswordCorrect(const std::wstring& password)
{
	return (0 == (this->password.compare(password) ));
}

void Credentials::clear()
{
	login.clear();
	password.clear();
	means.clear();
	deviation.clear();
}