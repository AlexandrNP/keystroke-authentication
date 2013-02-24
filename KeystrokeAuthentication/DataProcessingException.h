#pragma once

#include "CustomException.h"

class DataProcessigException: public CustomException
{
public:
	explicit DataProcessigException(const std::wstring what_arg);
	explicit DataProcessigException(const wchar_t* what_arg);
};