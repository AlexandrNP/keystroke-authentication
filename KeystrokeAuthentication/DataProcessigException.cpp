#include "stdafx.h"
#include "DataProcessingException.h"

DataProcessigException::DataProcessigException(const std::wstring what_arg)
	:CustomException(what_arg)
{}

DataProcessigException::DataProcessigException(const wchar_t* what_arg)
	:CustomException(what_arg)
{}