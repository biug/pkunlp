#ifndef _WEIGHT_BASE_H
#define _WEIGHT_BASE_H

#include "stdafx.h"

#include <string>

class WeightBaseOld {
protected:
	std::string m_sReadPath;
	std::string m_sRecordPath;

public:
	WeightBaseOld(const std::string & sRead, const std::string & sRecord) :
		m_sReadPath(sRead), m_sRecordPath(sRecord) {}
	virtual ~WeightBaseOld() {};

	virtual void loadScores() = 0;
	virtual void saveScores() const = 0;
	virtual void computeAverageFeatureWeights(const int & round) = 0;
};

#endif
