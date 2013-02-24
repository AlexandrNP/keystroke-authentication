#pragma once

#include <vector>
#include <string>

std::vector<double> mean(const std::vector<std::vector<double>>& trainData);
std::vector<double> deviation(const std::vector<std::vector<double>>& trainData);
void split(std::vector<std::wstring>& output, const std::wstring& input, const std::wstring& token = L" ");
std::wstring stringToWString(const char* str);




//--------------------------------------
template <typename T>
std::wstring vectorToWString(const std::vector<T>& vector)
{
	std::stringstream ss;
	std::copy(vector.begin(), vector.end(), std::ostream_iterator<T>(ss," "));
	std::string str = ss.str();

	if("" != str)
	{
		str.pop_back();
	}

	return stringToWString(str.c_str());
}
