#ifndef _GRAPH_WEIGHT_BASE_H
#define _GRAPH_WEIGHT_BASE_H

#include "stdafx.h"

#include "graph_transition_macros.h"
#include "common/parser/weight_base.h"

class GraphWeightBase : public WeightBase {
protected:

	DWord * m_pWords;
	DPOSTag * m_pPOSTags;
	DLabel * m_pLabels;
	DSuperTag * m_pSuperTags;

	SuperTagCandidates * m_pSuperTagCandidatesOfWords;
	SuperTagCandidates * m_pSuperTagCandidatesOfPOSTags;

public:
	GraphWeightBase(const std::string & sRead, const std::string & sRecord,
			DWord * pWords, DPOSTag * pPOSTags, DLabel * pLables, DSuperTag * pSuperTags,
			SuperTagCandidates * wordsCandidates, SuperTagCandidates * postagsCandidates) :
		WeightBase(sRead, sRecord), m_pWords(pWords), m_pPOSTags(pPOSTags), m_pLabels(pLables), m_pSuperTags(pSuperTags),
		m_pSuperTagCandidatesOfWords(wordsCandidates), m_pSuperTagCandidatesOfPOSTags(postagsCandidates) {}
	virtual ~GraphWeightBase() {};
};

#endif
