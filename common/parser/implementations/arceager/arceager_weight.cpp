#include "stdafx.h"
#include <fstream>

#include "arceager_weight.h"

namespace arceager {
	Weight::Weight(const std::string & sRead, const std::string & sRecord, DWord * words, DPOSTag * postags, DLabel * labels) :
		WeightBaseOld(sRead, sRecord),
		m_pWords(words),
		m_pPOSTags(postags),
		m_pLabels(labels),
		m_mapSTw("StackWord"),
		m_mapSTt("StackTag"),
		m_mapSTwt("StackWordTag"),
		m_mapN0w("NextWord"),
		m_mapN0t("NextTag"),
		m_mapN0wt("NextWordTag"),
		m_mapS1w("Stack+1Word"),
		m_mapS1t("Stack+1Tag"),
		m_mapS1wt("Stack+1WordTag"),
		m_mapS2w("Stack+2Word"),
		m_mapS2t("Stack+2Tag"),
		m_mapS2wt("Stack+2WordTag"),
		m_mapN1w("Next+1Word"),
		m_mapN1t("Next+1Tag"),
		m_mapN1wt("Next+1WordTag"),
		m_mapN2w("Next+2Word"),
		m_mapN2t("Next+2Tag"),
		m_mapN2wt("Next+2WordTag"),
		m_mapSTHw("StackHeadWord"),
		m_mapSTHt("StackHeadTag"),
		m_mapSTi("StackLabel"),
		m_mapSTHHw("StackHeadHeadWord"),
		m_mapSTHHt("StackHeadHeadTag"),
		m_mapSTHi("StackLabel"),
		m_mapSTLDw("StackLDWord"),
		m_mapSTLDt("StackLDTag"),
		m_mapSTLDi("StackLDLabel"),
		m_mapSTRDw("StackRDWord"),
		m_mapSTRDt("StackRDTag"),
		m_mapSTRDi("StackRDLabel"),
		m_mapN0LDw("NextLDWord"),
		m_mapN0LDt("NextLDTag"),
		m_mapN0LDi("NextLDLabel"),
		m_mapSTL2Dw("StackL2DWord"),
		m_mapSTL2Dt("StackL2DTag"),
		m_mapSTL2Di("StackL2DLabel"),
		m_mapSTR2Dw("StackR2DWord"),
		m_mapSTR2Dt("StackR2DTag"),
		m_mapSTR2Di("StackR2DLabel"),
		m_mapN0L2Dw("NextL2DWord"),
		m_mapN0L2Dt("NextL2DTag"),
		m_mapN0L2Di("NextL2DLabel"),
		m_mapHTw("HeadStackWord"),
		m_mapHTt("HeadStackTag"),
		m_mapHTwt("HeadStackWordTag"),
		m_mapSTwtN0wt("StackWordTagNextWordTag"),
		m_mapSTwtN0w("StackWordTagNextWord"),
		m_mapSTwN0wt("StackWordNextWordTag"),
		m_mapSTtN0wt("StackTagNextWordTag"),
		m_mapSTwtN0t("StackWordTagNextTag"),
		m_mapSTwN0w("StackWordNextWord"),
		m_mapSTtN0t("StackTagNextTag"),
		m_mapN0tN1t("NextTagNext+1Tag"),
		m_mapN0tN1tN2t("NextTagTrigram"),
		m_mapSTtN0tN1t("StackTagNextTagNext+1Tag"),
		m_mapSTtN0tN0LDt("StackTagNextTagNextLDTag"),
		m_mapN0tN0LDtN0L2Dt("StackTagNextTagNextLDTagNextTagNextL2DTag"),
		m_mapSTHtSTtN0t("StackHeadTagStackTagNextTag"),
		m_mapHTtHT2tN0t("HeadStackTagHeadStack2TagNextTag"),
		m_mapSTHHtSTHtSTt("StackHeadHeadTagStackHeadTagStackTag"),
		m_mapSTtSTLDtN0t("StackTagStackLDTagNextTag"),
		m_mapSTtSTLDtSTL2Dt("StackTagStackLDTagStackL2DTag"),
		m_mapSTtSTRDtN0t("StackTagStackRDTagNextTag"),
		m_mapSTtSTRDtSTR2Dt("StackTagStackRDTagStackR2DTag"),
		m_mapSTwd("StackWordDist"),
		m_mapSTtd("StackTagDist"),
		m_mapN0wd("NextWordDist"),
		m_mapN0td("NextTagDist"),
		m_mapSTwN0wd("StackWordNextWordDist"),
		m_mapSTtN0td("StackTagNextTagDist"),
		m_mapSTwra("StackWordRightArity"),
		m_mapSTtra("StackTagRightArity"),
		m_mapSTwla("StackWordLeftArity"),
		m_mapSTtla("StackTagLeftArity"),
		m_mapN0wla("NextWordRightArity"),
		m_mapN0tla("NextTagRightArity"),
		m_mapSTwrp("StackWordRightSetoftags"),
		m_mapSTtrp("StackTagRightSetoftags"),
		m_mapSTwlp("StackWordLeftSetoftags"),
		m_mapSTtlp("StackTagLeftSetoftags"),
		m_mapN0wlp("Next0WordLeftSetoftags"),
		m_mapN0tlp("Next0TagLeftSetoftags")
	{
		loadScores();
	}

	Weight::~Weight() = default;

	void Weight::loadScores() {

		if (m_sReadPath.empty()) {
			std::cout << "empty path" << std::endl;
			return;
		}
		std::ifstream input(m_sReadPath);
		if (!input) {
			std::cout << m_sReadPath << std::endl;
			std::cout << "bad path" << std::endl;
			return;
		}

		input >> *m_pWords;

		input >> *m_pPOSTags;

		input >> *m_pLabels;

		input >> m_mapSTw;
		input >> m_mapSTt;
		input >> m_mapSTwt;
		input >> m_mapN0w;
		input >> m_mapN0t;
		input >> m_mapN0wt;
		input >> m_mapS1w;
		input >> m_mapS1t;
		input >> m_mapS1wt;
		input >> m_mapS2w;
		input >> m_mapS2t;
		input >> m_mapS2wt;
		input >> m_mapN1w;
		input >> m_mapN1t;
		input >> m_mapN1wt;
		input >> m_mapN2w;
		input >> m_mapN2t;
		input >> m_mapN2wt;
		input >> m_mapSTHw;
		input >> m_mapSTHt;
		input >> m_mapSTi;
		input >> m_mapSTHHw;
		input >> m_mapSTHHt;
		input >> m_mapSTHi;
		input >> m_mapSTLDw;
		input >> m_mapSTLDt;
		input >> m_mapSTLDi;
		input >> m_mapSTRDw;
		input >> m_mapSTRDt;
		input >> m_mapSTRDi;
		input >> m_mapN0LDw;
		input >> m_mapN0LDt;
		input >> m_mapN0LDi;
		input >> m_mapSTL2Dw;
		input >> m_mapSTL2Dt;
		input >> m_mapSTL2Di;
		input >> m_mapSTR2Dw;
		input >> m_mapSTR2Dt;
		input >> m_mapSTR2Di;
		input >> m_mapN0L2Dw;
		input >> m_mapN0L2Dt;
		input >> m_mapN0L2Di;
		input >> m_mapHTw;
		input >> m_mapHTt;
		input >> m_mapHTwt;
		input >> m_mapSTwtN0wt;
		input >> m_mapSTwtN0w;
		input >> m_mapSTwN0wt;
		input >> m_mapSTtN0wt;
		input >> m_mapSTwtN0t;
		input >> m_mapSTwN0w;
		input >> m_mapSTtN0t;
		input >> m_mapN0tN1t;
		input >> m_mapN0tN1tN2t;
		input >> m_mapSTtN0tN1t;
		input >> m_mapSTtN0tN0LDt;
		input >> m_mapN0tN0LDtN0L2Dt;
		input >> m_mapSTHtSTtN0t;
		input >> m_mapHTtHT2tN0t;
		input >> m_mapSTHHtSTHtSTt;
		input >> m_mapSTtSTLDtN0t;
		input >> m_mapSTtSTLDtSTL2Dt;
		input >> m_mapSTtSTRDtN0t;
		input >> m_mapSTtSTRDtSTR2Dt;
		input >> m_mapSTwd;
		input >> m_mapSTtd;
		input >> m_mapN0wd;
		input >> m_mapN0td;
		input >> m_mapSTwN0wd;
		input >> m_mapSTtN0td;
		input >> m_mapSTwra;
		input >> m_mapSTtra;
		input >> m_mapSTwla;
		input >> m_mapSTtla;
		input >> m_mapN0wla;
		input >> m_mapN0tla;
		input >> m_mapSTwrp;
		input >> m_mapSTtrp;
		input >> m_mapSTwlp;
		input >> m_mapSTtlp;
		input >> m_mapN0wlp;
		input >> m_mapN0tlp;

		input.close();
	}

	void Weight::saveScores() const {

		if (m_sRecordPath.empty()) {
			return;
		}
		std::ofstream output(m_sRecordPath);
		if (!output) {
			return;
		}

		output << *m_pWords;

		output << *m_pPOSTags;

		output << *m_pLabels;

		output << m_mapSTw;
		output << m_mapSTt;
		output << m_mapSTwt;
		output << m_mapN0w;
		output << m_mapN0t;
		output << m_mapN0wt;
		output << m_mapS1w;
		output << m_mapS1t;
		output << m_mapS1wt;
		output << m_mapS2w;
		output << m_mapS2t;
		output << m_mapS2wt;
		output << m_mapN1w;
		output << m_mapN1t;
		output << m_mapN1wt;
		output << m_mapN2w;
		output << m_mapN2t;
		output << m_mapN2wt;
		output << m_mapSTHw;
		output << m_mapSTHt;
		output << m_mapSTi;
		output << m_mapSTHHw;
		output << m_mapSTHHt;
		output << m_mapSTHi;
		output << m_mapSTLDw;
		output << m_mapSTLDt;
		output << m_mapSTLDi;
		output << m_mapSTRDw;
		output << m_mapSTRDt;
		output << m_mapSTRDi;
		output << m_mapN0LDw;
		output << m_mapN0LDt;
		output << m_mapN0LDi;
		output << m_mapSTL2Dw;
		output << m_mapSTL2Dt;
		output << m_mapSTL2Di;
		output << m_mapSTR2Dw;
		output << m_mapSTR2Dt;
		output << m_mapSTR2Di;
		output << m_mapN0L2Dw;
		output << m_mapN0L2Dt;
		output << m_mapN0L2Di;
		output << m_mapHTw;
		output << m_mapHTt;
		output << m_mapHTwt;
		output << m_mapSTwtN0wt;
		output << m_mapSTwtN0w;
		output << m_mapSTwN0wt;
		output << m_mapSTtN0wt;
		output << m_mapSTwtN0t;
		output << m_mapSTwN0w;
		output << m_mapSTtN0t;
		output << m_mapN0tN1t;
		output << m_mapN0tN1tN2t;
		output << m_mapSTtN0tN1t;
		output << m_mapSTtN0tN0LDt;
		output << m_mapN0tN0LDtN0L2Dt;
		output << m_mapSTHtSTtN0t;
		output << m_mapHTtHT2tN0t;
		output << m_mapSTHHtSTHtSTt;
		output << m_mapSTtSTLDtN0t;
		output << m_mapSTtSTLDtSTL2Dt;
		output << m_mapSTtSTRDtN0t;
		output << m_mapSTtSTRDtSTR2Dt;
		output << m_mapSTwd;
		output << m_mapSTtd;
		output << m_mapN0wd;
		output << m_mapN0td;
		output << m_mapSTwN0wd;
		output << m_mapSTtN0td;
		output << m_mapSTwra;
		output << m_mapSTtra;
		output << m_mapSTwla;
		output << m_mapSTtla;
		output << m_mapN0wla;
		output << m_mapN0tla;
		output << m_mapSTwrp;
		output << m_mapSTtrp;
		output << m_mapSTwlp;
		output << m_mapSTtlp;
		output << m_mapN0wlp;
		output << m_mapN0tlp;

		output.close();
	}

	void Weight::computeAverageFeatureWeights(const int & round) {
		m_mapSTw.computeAverage(round);
		m_mapSTt.computeAverage(round);
		m_mapSTwt.computeAverage(round);
		m_mapN0w.computeAverage(round);
		m_mapN0t.computeAverage(round);
		m_mapN0wt.computeAverage(round);
		m_mapS1w.computeAverage(round);
		m_mapS1t.computeAverage(round);
		m_mapS1wt.computeAverage(round);
		m_mapS2w.computeAverage(round);
		m_mapS2t.computeAverage(round);
		m_mapS2wt.computeAverage(round);
		m_mapN1w.computeAverage(round);
		m_mapN1t.computeAverage(round);
		m_mapN1wt.computeAverage(round);
		m_mapN2w.computeAverage(round);
		m_mapN2t.computeAverage(round);
		m_mapN2wt.computeAverage(round);
		m_mapSTHw.computeAverage(round);
		m_mapSTHt.computeAverage(round);
		m_mapSTi.computeAverage(round);
		m_mapSTHHw.computeAverage(round);
		m_mapSTHHt.computeAverage(round);
		m_mapSTHi.computeAverage(round);
		m_mapSTLDw.computeAverage(round);
		m_mapSTLDt.computeAverage(round);
		m_mapSTLDi.computeAverage(round);
		m_mapSTRDw.computeAverage(round);
		m_mapSTRDt.computeAverage(round);
		m_mapSTRDi.computeAverage(round);
		m_mapN0LDw.computeAverage(round);
		m_mapN0LDt.computeAverage(round);
		m_mapN0LDi.computeAverage(round);
		m_mapSTL2Dw.computeAverage(round);
		m_mapSTL2Dt.computeAverage(round);
		m_mapSTL2Di.computeAverage(round);
		m_mapSTR2Dw.computeAverage(round);
		m_mapSTR2Dt.computeAverage(round);
		m_mapSTR2Di.computeAverage(round);
		m_mapN0L2Dw.computeAverage(round);
		m_mapN0L2Dt.computeAverage(round);
		m_mapN0L2Di.computeAverage(round);
		m_mapHTw.computeAverage(round);
		m_mapHTt.computeAverage(round);
		m_mapHTwt.computeAverage(round);
		m_mapSTwtN0wt.computeAverage(round);
		m_mapSTwtN0w.computeAverage(round);
		m_mapSTwN0wt.computeAverage(round);
		m_mapSTtN0wt.computeAverage(round);
		m_mapSTwtN0t.computeAverage(round);
		m_mapSTwN0w.computeAverage(round);
		m_mapSTtN0t.computeAverage(round);
		m_mapN0tN1t.computeAverage(round);
		m_mapN0tN1tN2t.computeAverage(round);
		m_mapSTtN0tN1t.computeAverage(round);
		m_mapSTtN0tN0LDt.computeAverage(round);
		m_mapN0tN0LDtN0L2Dt.computeAverage(round);
		m_mapSTHtSTtN0t.computeAverage(round);
		m_mapHTtHT2tN0t.computeAverage(round);
		m_mapSTHHtSTHtSTt.computeAverage(round);
		m_mapSTtSTLDtN0t.computeAverage(round);
		m_mapSTtSTLDtSTL2Dt.computeAverage(round);
		m_mapSTtSTRDtN0t.computeAverage(round);
		m_mapSTtSTRDtSTR2Dt.computeAverage(round);
		m_mapSTwd.computeAverage(round);
		m_mapSTtd.computeAverage(round);
		m_mapN0wd.computeAverage(round);
		m_mapN0td.computeAverage(round);
		m_mapSTwN0wd.computeAverage(round);
		m_mapSTtN0td.computeAverage(round);
		m_mapSTwra.computeAverage(round);
		m_mapSTtra.computeAverage(round);
		m_mapSTwla.computeAverage(round);
		m_mapSTtla.computeAverage(round);
		m_mapN0wla.computeAverage(round);
		m_mapN0tla.computeAverage(round);
		m_mapSTwrp.computeAverage(round);
		m_mapSTtrp.computeAverage(round);
		m_mapSTwlp.computeAverage(round);
		m_mapSTtlp.computeAverage(round);
		m_mapN0wlp.computeAverage(round);
		m_mapN0tlp.computeAverage(round);
	}
}
