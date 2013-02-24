#pragma once

#include "CustomException.h"

class XmlParsingException: public CustomException
{
	public:
		explicit XmlParsingException(const std::wstring what_arg);
		explicit XmlParsingException(const wchar_t* what_arg);
};