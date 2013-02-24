#pragma  once

#include "DataProcessingException.h"

#include <vector>

class AnomalyDetector
{
	public:
		AnomalyDetector();
		AnomalyDetector(const std::vector<std::vector<double>>& trainData);
		void train(const std::vector<std::vector<double>>& trainData);
		bool isNormal(const std::vector<double>& data);
		void setMeans(const std::vector<double>& means);
		void setDeviation(const std::vector<double>& deviation);
		void setThreshold(const double threshold);
		std::vector<double> getMeans();
		std::vector<double> getDeviation();
		double getThreshold();
	private:
		void createThreshold(const std::vector<std::vector<double>>& trainData);
		double countAnomalyScore(const std::vector<double> data);
		std::vector<double> m_means;
		std::vector<double> m_deviation;
		double m_threshold;
};