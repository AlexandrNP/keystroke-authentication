#pragma once

#include "stdafx.h"

#include "AnomalyDetector.h"
#include "AuxiliaryFunctions.h"



AnomalyDetector::AnomalyDetector()
	:m_threshold(0)
{}


//-----------------------------------------
AnomalyDetector::AnomalyDetector(const std::vector<std::vector<double>>& trainData)
{
	train(trainData);
}



//-----------------------------------------
void AnomalyDetector::train(const std::vector<std::vector<double>>& trainData)
{
	try
	{
		m_means = mean(trainData);
		m_deviation = deviation(trainData);
	}
	catch(DataProcessigException err)
	{
		throw DataProcessigException(err);
	}

	createThreshold(trainData);
}

//-----------------------------------------
double AnomalyDetector::countAnomalyScore(const std::vector<double> data)
{
	double anomalyScore = 0;
	for(unsigned int i = 0; i < m_means.size(); ++i)
	{
		anomalyScore += std::abs(data[i] - m_means[i]) / m_deviation[i];
	}
	return anomalyScore;
}


//-----------------------------------------
void AnomalyDetector::createThreshold(const std::vector<std::vector<double>>& trainData)
{
	m_threshold = 0;
	double anomalyScore = 0;
	double maxAlomalyScore = 0,
		     secondAlomalyScore = 0;
	for(auto it = trainData.cbegin(); it!= trainData.cend(); ++it)
	{
		anomalyScore = countAnomalyScore(*it);

		if(anomalyScore > maxAlomalyScore)
		{
			maxAlomalyScore = anomalyScore;
		}
		else if (anomalyScore > secondAlomalyScore)
		{
			secondAlomalyScore = anomalyScore;
		}

		anomalyScore = 0;
	}

	m_threshold += (maxAlomalyScore + secondAlomalyScore) / 2;
}


//-----------------------------------------
bool AnomalyDetector::isNormal(const std::vector<double>& data)
{
	if(data.size() != m_means.size())
	{
		return false;
	}

	double anomalyScore = countAnomalyScore(data);
	return anomalyScore < m_threshold;
}

//-----------------------------------------
std::vector<double> AnomalyDetector::getMeans()
{
	return m_means;
}


//-----------------------------------------
double AnomalyDetector::getThreshold()
{
	return m_threshold;
}

//-----------------------------------------
std::vector<double> AnomalyDetector::getDeviation()
{
	return m_deviation;
}

//------------------------------------------
void AnomalyDetector::setMeans(std::vector<double>& means)
{
	m_means = means;
}

//------------------------------------------
void AnomalyDetector::setDeviation(std::vector<double>& deviation)
{
	m_deviation = deviation;
}


//------------------------------------------
void AnomalyDetector::setThreshold(double threshold)
{
	m_threshold = threshold;
}