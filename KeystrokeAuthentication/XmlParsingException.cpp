#include "stdafx.h"
#include "XmlParsingException.h"


//---------------------------------
XmlParsingException::XmlParsingException(const std::wstring what_arg)
	:CustomException(what_arg)
{}


//--------------------------------
XmlParsingException::XmlParsingException(const wchar_t* what_arg)
	:CustomException(what_arg)
{}