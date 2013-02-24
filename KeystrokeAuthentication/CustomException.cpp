#include "stdafx.h"
#include "CustomException.h"


//---------------------------------
CustomException::CustomException(const std::wstring what_arg)
	:m_reason(what_arg)
{}


//--------------------------------
CustomException::CustomException(const wchar_t* what_arg)
	:m_reason(what_arg)
{}


//--------------------------------
std::wstring CustomException::getReason()
{
	return m_reason;
}