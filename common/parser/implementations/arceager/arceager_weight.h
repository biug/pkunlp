#ifndef _ARCEAGER_WEIGHT_H
#define _ARCEAGER_WEIGHT_H

#include "stdafx.h"

#include "arceager_macros.h"
#include "common/parser/weight_base.h"

namespace arceager {
	class Weight : public WeightBase {
	public:

		DWord * m_pWords;
		DPOSTag * m_pPOSTags;
		DLabel * m_pLabels;

		WordMap m_mapSTw;
		POSTagMap m_mapSTt;
		POSTaggedWordMap m_mapSTwt;

		WordMap m_mapN0w;
		POSTagMap m_mapN0t;
		POSTaggedWordMap m_mapN0wt;

		WordMap m_mapS1w;
		POSTagMap m_mapS1t;
		POSTaggedWordMap m_mapS1wt;

		WordMap m_mapS2w;
		POSTagMap m_mapS2t;
		POSTaggedWordMap m_mapS2wt;

		WordMap m_mapN1w;
		POSTagMap m_mapN1t;
		POSTaggedWordMap m_mapN1wt;

		WordMap m_mapN2w;
		POSTagMap m_mapN2t;
		POSTaggedWordMap m_mapN2wt;

		WordMap m_mapSTHw;
		POSTagMap m_mapSTHt;
		IntMap m_mapSTi;

		WordMap m_mapSTHHw;
		POSTagMap m_mapSTHHt;
		IntMap m_mapSTHi;

		WordMap m_mapSTLDw;
		POSTagMap m_mapSTLDt;
		IntMap m_mapSTLDi;

		WordMap m_mapSTRDw;
		POSTagMap m_mapSTRDt;
		IntMap m_mapSTRDi;

		WordMap m_mapN0LDw;
		POSTagMap m_mapN0LDt;
		IntMap m_mapN0LDi;

		WordMap m_mapSTL2Dw;
		POSTagMap m_mapSTL2Dt;
		IntMap m_mapSTL2Di;

		WordMap m_mapSTR2Dw;
		POSTagMap m_mapSTR2Dt;
		IntMap m_mapSTR2Di;

		WordMap m_mapN0L2Dw;
		POSTagMap m_mapN0L2Dt;
		IntMap m_mapN0L2Di;

		WordMap m_mapHTw;
		POSTagMap m_mapHTt;
		POSTaggedWordMap m_mapHTwt;

		TwoPOSTaggedWordsMap m_mapSTwtN0wt;
		WordWordPOSTagMap m_mapSTwtN0w;
		WordWordPOSTagMap m_mapSTwN0wt;
		WordPOSTagPOSTagMap m_mapSTtN0wt;
		WordPOSTagPOSTagMap m_mapSTwtN0t;
		TwoWordsMap m_mapSTwN0w;

		POSTagSet2Map m_mapSTtN0t;
		POSTagSet2Map m_mapN0tN1t;
		POSTagSet3Map m_mapN0tN1tN2t;
		POSTagSet3Map m_mapSTtN0tN1t;
		POSTagSet3Map m_mapSTtN0tN0LDt;
		POSTagSet3Map m_mapN0tN0LDtN0L2Dt;
		POSTagSet3Map m_mapSTHtSTtN0t;
		POSTagSet3Map m_mapHTtHT2tN0t;
		POSTagSet3Map m_mapSTHHtSTHtSTt;
		POSTagSet3Map m_mapSTtSTLDtN0t;
		POSTagSet3Map m_mapSTtSTLDtSTL2Dt;
		POSTagSet3Map m_mapSTtSTRDtN0t;
		POSTagSet3Map m_mapSTtSTRDtSTR2Dt;

		WordIntMap m_mapSTwd;
		POSTagIntMap m_mapSTtd;
		WordIntMap m_mapN0wd;
		POSTagIntMap m_mapN0td;
		WordWordIntMap m_mapSTwN0wd;
		POSTagPOSTagIntMap m_mapSTtN0td;

		WordIntMap m_mapSTwra;
		POSTagIntMap m_mapSTtra;
		WordIntMap m_mapSTwla;
		POSTagIntMap m_mapSTtla;
		WordIntMap m_mapN0wla;
		POSTagIntMap m_mapN0tla;

		WordSetOfDepLabelsMap m_mapSTwrp;
		POSTagSetOfDepLabelsMap m_mapSTtrp;
		WordSetOfDepLabelsMap m_mapSTwlp;
		POSTagSetOfDepLabelsMap m_mapSTtlp;
		WordSetOfDepLabelsMap m_mapN0wlp;
		POSTagSetOfDepLabelsMap m_mapN0tlp;

	public:
		Weight(const std::string & sRead, const std::string & sRecord, DWord * words, DPOSTag * postags, DLabel * labels);
		~Weight();

		void loadScores() override;
		void saveScores() const override;
		void computeAverageFeatureWeights(const int & round) override;
	};
}

#endif
