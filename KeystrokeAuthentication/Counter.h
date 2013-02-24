#pragma once


class Counter
{
	public:
		Counter();
		void startCounter();
		double getCounter();

	private:
		__int64 m_counterStart;
		double m_PCFreq;
};