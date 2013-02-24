#pragma once

#include "objbase.h"
#include "Credentials.h"
#include "XmlParsingException.h"

#include <vector>
#include <xmllite.h>
#include <atlbase.h>

class XmlCredentialsParser
{
	public:
		XmlCredentialsParser();
		void setFilePath(const std::wstring& filePath);
		Credentials getCredentialsByLogin(const std::wstring& login);
		void writeCredentials(Credentials& credentials);
	private:
		void readAttributes(Credentials& credentials);
		void openReadStream(const std::wstring& filePath);
		void openWriteStream(const std::wstring& filePath);
		void seekBeginOfStream();
		void seekEndOfStream();
		void setReaderStream();
		void setWriterStream();
		void closeStream();
		CComPtr<IXmlReader> mp_reader;
		CComPtr<IXmlWriter> mp_writer;
		CComPtr<IStream> mp_stream;
		std::wstring m_filePath;
		std::wstring m_loginAttribute;
		std::wstring m_passwordAttribute;
		std::wstring m_meansAttribute;
		std::wstring m_deviationAttribute;
		std::wstring m_threshold;
};