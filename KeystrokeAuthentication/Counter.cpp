#include "stdafx.h"
#include "Counter.h"

#include <windows.h>

Counter::Counter()
{
	LARGE_INTEGER li;
	QueryPerformanceFrequency(&li);
	m_PCFreq = double(li.QuadPart)/1000.0;
	m_counterStart = 0;
}

void Counter::startCounter()
{
	LARGE_INTEGER li;
	QueryPerformanceCounter(&li);
	m_counterStart = li.QuadPart;
}


double Counter::getCounter()
{
	LARGE_INTEGER li;
	QueryPerformanceCounter(&li);
	return double(li.QuadPart-m_counterStart)/m_PCFreq;
}