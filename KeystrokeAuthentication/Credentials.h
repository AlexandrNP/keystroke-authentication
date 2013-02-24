#pragma once

#include <string>
#include <vector>

struct Credentials
{
	public:
		bool isPasswordCorrect(const std::wstring& password);
		void clear();
		std::wstring login;
		std::wstring password;
		std::vector<double> means;
		std::vector<double> deviation;
		double threshold;
};