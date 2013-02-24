#include "stdafx.h"
#include "AuxiliaryFunctions.h"
#include "DataProcessingException.h"

#include <algorithm>
#include <Shlwapi.h>

double square(const double value)
{
	return value * value;
}

std::vector<double> mean(const std::vector<std::vector<double>>& trainData)
{
	if(0 == trainData.size())
	{
		throw DataProcessigException(L"Unable to compute means. No data was passed.");
	}

	std::vector<double> means;
	means.resize(trainData.cbegin()->size());
	std::fill(means.begin(), means.end(), 0);

	std::for_each(trainData.cbegin(), trainData.cend(), [&means](const std::vector<double>& vec)->void
	{
		for(unsigned int i = 0; i < means.size(); ++i)
		{
			means[i] += vec[i];
		}
	});

	for(auto it = means.begin(); it != means.cend(); ++it)
	{
		(*it) /= trainData.size();
	}

	return means;
}


//------------------------------------
std::vector<double> deviation(const std::vector<std::vector<double>>& trainData)
{
	if(1 >= trainData.size())
	{
		throw DataProcessigException(L"Unable to compute variance from only one data sample.");
	}

	std::vector<double> means = mean(trainData);

	std::vector<double> deviations;
	deviations.resize(trainData.cbegin()->size());
	std::fill(deviations.begin(), deviations.end(), 0);

	std::for_each(trainData.cbegin(), trainData.cend(), [means, &deviations](const std::vector<double>& vec)->void
	{
		for(unsigned int i = 0; i < deviations.size(); ++i)
		{
			deviations[i] += square(vec[i] - means[i]);
		}
	});

	for(auto it = deviations.begin(); it != deviations.end(); ++it)
	{
		(*it) = sqrt(*it);
		(*it) /= std::sqrt((double) trainData.size() - 1);
	}

	return deviations;
}

//-------------------------------------------

void split(std::vector<std::wstring>& output, const std::wstring& input, const std::wstring& token)
{
	output.clear();
	size_t begin = 0, end = 0;
	while(end != std::string::npos)
	{
		end = input.find(token, begin);
		std::wstring substr = input.substr(begin, end - begin);
		output.push_back(substr);
		begin = end + 1;
	}
}

//----------------------------

std::wstring stringToWString(const char* str)
{
	int Len = static_cast<int>(::strlen(str) + 1);
	std::vector<wchar_t> Buffer(Len);
	::MultiByteToWideChar(CP_ACP, 0, str, -1, &Buffer.front(), Len);
	return &Buffer.front();
}