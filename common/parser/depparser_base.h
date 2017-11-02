#ifndef _DEPPARSER_BASE_H
#define _DEPPARSER_BASE_H

#include "stdafx.h"

#include "include/learning/perceptron/score.h"
#include "common/parser/old_weight_base.h"

enum ParserState {
	TRAIN = 1,
	PARSE,
	GOLDTEST
};

class DepParserBase {
protected:
	WeightBaseOld* m_Weight;
	int m_nState;

public:
	int m_nTotalErrors;
	int m_nScoreIndex;
	int m_nTrainingRound;

public:
	DepParserBase(int nState) :
		m_Weight(nullptr), m_nState(nState), m_nTotalErrors(0), m_nScoreIndex(nState == ParserState::TRAIN ? ScoreType::eNonAverage : ScoreType::eAverage), m_nTrainingRound(0) {}
	virtual ~DepParserBase() {};

	virtual void finishtraining() {
		m_Weight->computeAverageFeatureWeights(m_nTrainingRound);
		m_Weight->saveScores();
		std::cout << "Total number of training errors are: " << m_nTotalErrors << std::endl;
	}
};

#endif
