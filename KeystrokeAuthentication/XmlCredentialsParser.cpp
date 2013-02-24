#include "stdafx.h"
#include "XmlCredentialsParser.h"
#include "AuxiliaryFunctions.h"

#include <Shlwapi.h>
#include <iterator>
#include <vector>
#include <exception>
#include <sstream>
#include <fstream>


//----------------------------
XmlCredentialsParser::XmlCredentialsParser() : m_loginAttribute(L"login"),
														m_passwordAttribute(L"password"),
														m_meansAttribute(L"means"),
														m_deviationAttribute(L"deviation"),
														m_threshold(L"threshold"),
														mp_stream(nullptr)
{
	if (CreateXmlReader(__uuidof(IXmlReader),	reinterpret_cast<void**>(&mp_reader), 0) == S_FALSE)
	{
		throw XmlParsingException(L"Unable to create XmlReader");
	}

	if (CreateXmlWriter(__uuidof(IXmlWriter),	reinterpret_cast<void**>(&mp_writer),0) == S_FALSE)
	{
		throw XmlParsingException(L"Unable to create XmlReader");
	}
}



//----------------------------
void XmlCredentialsParser::setFilePath(const std::wstring& filePath)
{
	m_filePath = filePath;
}



//----------------------------
void XmlCredentialsParser::setReaderStream()
{
	if (mp_reader->SetInput(mp_stream) == S_FALSE)
	{
		throw XmlParsingException(L"Unable to set stream to reader");
	}
}



//----------------------------
void XmlCredentialsParser::setWriterStream()
{
	if(mp_writer->SetOutput(mp_stream) == S_FALSE)
	{
		throw XmlParsingException(L"Unable to set stream to reader");
	}
}



//----------------------------
void XmlCredentialsParser::openReadStream(const std::wstring& filePath)
{
	std::wifstream stream;
	stream.open(filePath);
	std::istreambuf_iterator<wchar_t> end;
	std::wstring fileString(std::istreambuf_iterator<wchar_t>(stream), end);
	stream.close();
	std::wstring xmlString = L"<body>" + fileString + L"</body>";

	if (CreateStreamOnHGlobal(NULL, false,	&mp_stream) == S_FALSE)
	{
		throw XmlParsingException(L"Unable to create file stream");
	}

	DWORD dwWritten = 0;
	mp_stream->Write(xmlString.c_str(), wcslen(xmlString.c_str()) * sizeof(wchar_t), &dwWritten);
	seekBeginOfStream();
}



//----------------------------
void XmlCredentialsParser::openWriteStream(const std::wstring& filePath)
{
	if (SHCreateStreamOnFile(filePath.c_str(), STGM_WRITE,	&mp_stream) == S_FALSE)
	{
		if (SHCreateStreamOnFile(filePath.c_str(), STGM_CREATE | STGM_WRITE,	&mp_stream) == S_FALSE)
		{
			throw XmlParsingException(L"Unable to create file stream");
		}
	}
}



//----------------------------
void XmlCredentialsParser::closeStream()
{
	mp_stream.Release();
}



//----------------------------
void XmlCredentialsParser::readAttributes(Credentials& credentials)
{
	const wchar_t* buffer;
	std::vector<std::wstring> raw_means;
	std::vector<std::wstring> raw_deviation;

	mp_reader->GetValue(&buffer, NULL);
	credentials.login = buffer;

	if(S_OK == mp_reader->MoveToAttributeByName(m_passwordAttribute.c_str(), NULL))
	{
		mp_reader->GetValue(&buffer, NULL);
		credentials.password = buffer;
	}

	if(S_OK == mp_reader->MoveToAttributeByName(m_meansAttribute.c_str(), NULL))
	{
		mp_reader->GetValue(&buffer, NULL);
		split(raw_means, buffer);
		for(auto it = raw_means.cbegin(); it != raw_means.cend(); ++it)
		{
			credentials.means.push_back(std::stod(*it));
		}
	}

	if(S_OK == mp_reader->MoveToAttributeByName(m_deviationAttribute.c_str(), NULL))
	{
		mp_reader->GetValue(&buffer, NULL);
		split(raw_deviation, buffer);
		for(auto it = raw_deviation.cbegin(); it != raw_deviation.cend(); ++it)
		{
			credentials.deviation.push_back(std::stod(*it));
		}
	}

	if(S_OK == mp_reader->MoveToAttributeByName(m_threshold.c_str(), NULL))
	{
		mp_reader->GetValue(&buffer, NULL);
		credentials.threshold = std::stod(buffer);
	}
}



//--------------------
Credentials XmlCredentialsParser::getCredentialsByLogin(const std::wstring& login)
{
	openReadStream(m_filePath);
	setReaderStream();

	Credentials credentials;
	HRESULT result = S_OK;
	XmlNodeType nodeType = XmlNodeType_None;
	const wchar_t* buffer;

	if (S_FALSE == mp_reader->SetProperty(XmlReaderProperty_DtdProcessing, DtdProcessing_Prohibit)) 
	{ 
		throw XmlParsingException(L"Unable to set properties for IXmlReader");
	} 

	while (S_OK == (result = mp_reader->Read(&nodeType)))
	{
		if( S_OK == mp_reader->MoveToAttributeByName(m_loginAttribute.c_str(), NULL))
		{
			mp_reader->GetValue(&buffer, NULL);
			if(0 == login.compare(buffer))
			{
				readAttributes(credentials);
			}
		}
	}
	closeStream();
 return credentials;
}



//-------------------------------------
void XmlCredentialsParser::seekEndOfStream()
{
	if(nullptr == mp_stream)
	{
		throw XmlParsingException(L"Stream is not defined");
	}

	LARGE_INTEGER pos = {0, 0};
	pos.QuadPart = 0;

	if(S_FALSE == (mp_stream->Seek(pos, STREAM_SEEK_END, 0)))
	{
		throw XmlParsingException(L"Unable to seek end of stream");
	}
}



//-------------------------------------
void XmlCredentialsParser::seekBeginOfStream()
{
	if(nullptr == mp_stream)
	{
		throw XmlParsingException(L"Stream is not defined");
	}

	LARGE_INTEGER pos = {0, 0};
	pos.QuadPart = 0;

	if(S_FALSE == (mp_stream->Seek(pos, STREAM_SEEK_SET, 0)))
	{
		throw XmlParsingException(L"Unable to seek end of stream");
	}
}



//-------------------------------------
void XmlCredentialsParser::writeCredentials(Credentials& credentials)
{
	openWriteStream(m_filePath);
	setWriterStream();
	seekEndOfStream();
	

	if (S_FALSE == (mp_writer->SetProperty(XmlWriterProperty_Indent, TRUE))) 
	{ throw XmlParsingException(L"Unable to set property to IXmlWriter"); } 
		

	if(S_FALSE == (mp_writer->WriteWhitespace(L"\n")))
	{	throw XmlParsingException(L"Unable to write to file");}

	
	if(S_FALSE == mp_writer->WriteStartElement(0, L"credentials", 0))
	{	throw XmlParsingException(L"Unable to write to file"); }


	if(S_FALSE == mp_writer->WriteAttributeString(0, m_loginAttribute.c_str(), 0, 
																	credentials.login.c_str()))
	{	throw XmlParsingException(L"Unable to write to file");	}


	if(S_FALSE == mp_writer->WriteAttributeString(0, m_passwordAttribute.c_str(), 0, 
																	credentials.password.c_str()))
	{	throw XmlParsingException(L"Unable to write to file"); }


	std::wstring means = vectorToWString(credentials.means);
	if(S_FALSE == mp_writer->WriteAttributeString(0, m_meansAttribute.c_str(), 0, means.c_str()))
	{	throw XmlParsingException(L"Unable to write to file");}


	std::wstring deviation = vectorToWString(credentials.deviation);
	if(S_FALSE == mp_writer->WriteAttributeString(0, m_deviationAttribute.c_str(), 0, deviation.c_str()))
	{	throw XmlParsingException(L"Unable to write to file");}

	if(S_FALSE == mp_writer->WriteAttributeString(0, m_threshold.c_str(), 0, 
																std::to_wstring((long double) credentials.threshold).c_str()))
	{	throw XmlParsingException(L"Unable to write to file");}

	if(S_FALSE == mp_writer->WriteEndElement())
	{	throw XmlParsingException(L"Unable to write to file");}

	if(S_FALSE == mp_writer->WriteEndDocument())
	{	throw XmlParsingException(L"Unable to write to file");}

	mp_writer->Flush();
	closeStream();
}