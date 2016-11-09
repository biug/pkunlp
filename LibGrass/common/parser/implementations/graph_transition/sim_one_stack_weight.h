#ifndef _SIM_ONE_STACK_WEIGHT_H
#define _SIM_ONE_STACK_WEIGHT_H

#include "stdafx.h"

#include "graph_transition_weight.h"
#include "graph_transition_depparser.h"
#include "include/learning/perceptron/packed_score.h"

namespace graph_transition {

	template<class RET_TYPE, class STATE_TYPE, class ACTION_TYPE>
	class SimOneStackWeight : public GraphWeightBase {
	public:

		static WordPOSTag empty_taggedword;
		static WordPOSTag start_taggedword;
		static WordPOSTag end_taggedword;
		static TagSet empty_tagset;

		// feature variable
		Unsigned poses_feature;
		Int uni_feature;
		TwoInts bi_features;
		ThreeInts tri_features;
		FourInts quar_features;
		IntTagSet uni_tagset;
		TwoIntsTagSet bi_tagset;

		// uni-gram
		// st, n0, st2
		WordMap<RET_TYPE> m_mapSTw;
		POSTagMap<RET_TYPE> m_mapSTpt;
		WordMap<RET_TYPE> m_mapST2w;
		POSTagMap<RET_TYPE> m_mapST2pt;
		WordMap<RET_TYPE> m_mapN0w;
		POSTagMap<RET_TYPE> m_mapN0pt;

		POSTagIntMap<RET_TYPE> m_mapSTipt;
		POSTagSet2IntMap<RET_TYPE> m_mapSTiptSTjpt;
		POSTagIntMap<RET_TYPE> m_mapN0ipt;
		POSTagSet2IntMap<RET_TYPE> m_mapN0iptN0jpt;

		// bi-gram
		// st + n0
		TwoWordsMap<RET_TYPE> m_mapSTwN0w;
		WordPOSTagMap<RET_TYPE> m_mapSTwN0pt;
		WordPOSTagMap<RET_TYPE> m_mapSTptN0w;
		POSTagSet2Map<RET_TYPE> m_mapSTptN0pt;
		// st + n0 + dis
		POSTagSet2IntMap<RET_TYPE> m_mapSTptN0ptD;
		// st + n0 + st left/right head/pred
		POSTagSet2IntMap<RET_TYPE> m_mapSTptN0ptSTLPi;
		POSTagSet2IntMap<RET_TYPE> m_mapSTptN0ptSTRPi;
		POSTagSet2IntMap<RET_TYPE> m_mapSTptN0ptN0LPi;

		// st2 + n0
		TwoWordsMap<RET_TYPE> m_mapST2wN0w;
		WordPOSTagMap<RET_TYPE> m_mapST2wN0pt;
		WordPOSTagMap<RET_TYPE> m_mapST2ptN0w;
		POSTagSet2Map<RET_TYPE> m_mapST2ptN0pt;
		// st2 + n0 + dis
		POSTagSet2IntMap<RET_TYPE> m_mapST2ptN0ptD;
		// st2 + n0 + st2 left/right head/pred
		POSTagSet2IntMap<RET_TYPE> m_mapST2ptN0ptST2LPi;
		POSTagSet2IntMap<RET_TYPE> m_mapST2ptN0ptST2RPi;
		POSTagSet2IntMap<RET_TYPE> m_mapST2ptN0ptN0LPi;

		// st + st2
		TwoWordsMap<RET_TYPE> m_mapSTwST2w;
		WordPOSTagMap<RET_TYPE> m_mapSTwST2pt;
		WordPOSTagMap<RET_TYPE> m_mapSTptST2w;
		POSTagSet2Map<RET_TYPE> m_mapSTptST2pt;

		// tri-gram
		// st + n0 + st2
		POSTagSet3Map<RET_TYPE> m_mapSTptN0ptST2pt;

		// st + n0 + st left/right head/pred
		POSTagSet3IntMap<RET_TYPE> m_mapSTptN0ptSTLHptSTLHl;
		POSTagSet3IntMap<RET_TYPE> m_mapSTptN0ptSTLPptSTLPl;
		POSTagSet3IntMap<RET_TYPE> m_mapSTptN0ptSTRHptSTRHl;
		POSTagSet3IntMap<RET_TYPE> m_mapSTptN0ptSTRPptSTRPl;

		// st + n0 + n0 left head/pred
		POSTagSet3IntMap<RET_TYPE> m_mapSTptN0ptN0LHptN0LHl;
		POSTagSet3IntMap<RET_TYPE> m_mapSTptN0ptN0LPptN0LPl;

		// quar-gram
		// st + n0 + st right head + st right pred
		POSTagSet4TwoIntsMap<RET_TYPE> m_mapSTptN0ptSTRHptSTRPptSTRHlSTRPl;
		// st + n0 + st left pred + st left pred 2
		POSTagSet4TwoIntsMap<RET_TYPE> m_mapSTptN0ptSTLPptSTLP2ptSTLPlSTLP2l;
		// st + n0 + st right pred + st right pred 2
		POSTagSet4TwoIntsMap<RET_TYPE> m_mapSTptN0ptSTRPptSTRP2ptSTRPlSTRP2l;
		// st + n0 + n0 left head + n0 left pred
		POSTagSet4TwoIntsMap<RET_TYPE> m_mapSTptN0ptN0LHptN0LPptN0LHlN0LPl;
		// st + n0 + n0 left pred + n0 left pred 2
		POSTagSet4TwoIntsMap<RET_TYPE> m_mapSTptN0ptN0LPptN0LP2ptN0LPlN0LP2l;

		// st2 + n0 + st2 left/right head/pred
		POSTagSet3IntMap<RET_TYPE> m_mapST2ptN0ptST2LHptST2LHl;
		POSTagSet3IntMap<RET_TYPE> m_mapST2ptN0ptST2LPptST2LPl;
		POSTagSet3IntMap<RET_TYPE> m_mapST2ptN0ptST2RHptST2RHl;
		POSTagSet3IntMap<RET_TYPE> m_mapST2ptN0ptST2RPptST2RPl;

		// st + n0 + n0 left head/pred
		POSTagSet3IntMap<RET_TYPE> m_mapST2ptN0ptN0LHptN0LHl;
		POSTagSet3IntMap<RET_TYPE> m_mapST2ptN0ptN0LPptN0LPl;

		// quar-gram
		// st2 + n0 + st2 right head + st2 right pred
		POSTagSet4TwoIntsMap<RET_TYPE> m_mapST2ptN0ptST2RHptST2RPptST2RHlST2RPl;
		// st2 + n0 + st2 left pred + st2 left pred 2
		POSTagSet4TwoIntsMap<RET_TYPE> m_mapST2ptN0ptST2LPptST2LP2ptST2LPlST2LP2l;
		// st2 + n0 + st2 right pred + st2 right pred 2
		POSTagSet4TwoIntsMap<RET_TYPE> m_mapST2ptN0ptST2RPptST2RP2ptST2RPlST2RP2l;
		// st2 + n0 + n0 left head + n0 left pred
		POSTagSet4TwoIntsMap<RET_TYPE> m_mapST2ptN0ptN0LHptN0LPptN0LHlN0LPl;
		// st2 + n0 + n0 left pred + n0 left pred 2
		POSTagSet4TwoIntsMap<RET_TYPE> m_mapST2ptN0ptN0LPptN0LP2ptN0LPlN0LP2l;

		// st + n0 + label set
		POSTagSet2TagsetMap<RET_TYPE> m_mapSTptN0ptSTll;
		POSTagSet2TagsetMap<RET_TYPE> m_mapSTptN0ptSTrl;
		POSTagSet2TagsetMap<RET_TYPE> m_mapSTptN0ptN0ll;
		// st2 + n0 + label set
		POSTagSet2TagsetMap<RET_TYPE> m_mapST2ptN0ptST2ll;
		POSTagSet2TagsetMap<RET_TYPE> m_mapST2ptN0ptST2rl;
		POSTagSet2TagsetMap<RET_TYPE> m_mapST2ptN0ptN0ll;

		// char feature (for chinese)
		// st context char
		StringMap<RET_TYPE> m_map1CharBeforeST;
		StringMap<RET_TYPE> m_map2CharBeforeST;
		StringMap<RET_TYPE> m_map3CharBeforeST;
		StringMap<RET_TYPE> m_map1CharAfterST;
		StringMap<RET_TYPE> m_map2CharAfterST;
		StringMap<RET_TYPE> m_map3CharAfterST;
		// n0 context char
		StringMap<RET_TYPE> m_map1CharBeforeN0;
		StringMap<RET_TYPE> m_map2CharBeforeN0;
		StringMap<RET_TYPE> m_map3CharBeforeN0;
		StringMap<RET_TYPE> m_map1CharAfterN0;
		StringMap<RET_TYPE> m_map2CharAfterN0;
		StringMap<RET_TYPE> m_map3CharAfterN0;

		// path feature
		// st - n0
		StringMap<RET_TYPE> m_mapSTPOSPath;
		StringMap<RET_TYPE> m_mapSTFPOSPath;
		WordWordPOSTagMap<RET_TYPE> m_mapSTwN0wSTsynhpt;
		WordWordPOSTagMap<RET_TYPE> m_mapSTwN0wN0synhpt;
		// st2 - n0
		StringMap<RET_TYPE> m_mapST2POSPath;
		StringMap<RET_TYPE> m_mapST2FPOSPath;
		WordWordPOSTagMap<RET_TYPE> m_mapST2wN0wST2synhpt;
		WordWordPOSTagMap<RET_TYPE> m_mapST2wN0wN0synhpt;

		// supertag feature
		SuperTagMap<RET_TYPE> m_mapSTst;
		SuperTagMap<RET_TYPE> m_mapST2st;
		SuperTagIntMap<RET_TYPE> m_mapN0ist;
		SuperTagSet2Map<RET_TYPE> m_mapSTstST2st;
		WordSuperTagMap<RET_TYPE> m_mapSTstN0w;
		POSTagSuperTagMap<RET_TYPE> m_mapSTstN0pt;
		WordPOSTagSuperTagMap<RET_TYPE> m_mapSTstN0wpt;

	public:
		SimOneStackWeight(const std::string & sRead, const std::string & sRecord,
				DWord * pWords, DPOSTag * pPOSTags, DLabel * pLables, DSuperTag * pSuperTags,
				SuperTagCandidates * wordsCandidates, SuperTagCandidates * postagsCandidates);
		~SimOneStackWeight();

		void loadScores() override;
		void saveScores() const override;
		void computeAverageFeatureWeights(const int & round) override;

		void getOrUpdateFeatureScores(GraphDepParserBase<RET_TYPE, STATE_TYPE, ACTION_TYPE> * parser, const STATE_TYPE & state, const ActionScoreIncrement & amount);
	};


	template<class RET_TYPE, class STATE_TYPE, class ACTION_TYPE> WordPOSTag SimOneStackWeight<RET_TYPE, STATE_TYPE, ACTION_TYPE>::empty_taggedword = WordPOSTag();
	template<class RET_TYPE, class STATE_TYPE, class ACTION_TYPE> WordPOSTag SimOneStackWeight<RET_TYPE, STATE_TYPE, ACTION_TYPE>::start_taggedword = WordPOSTag();
	template<class RET_TYPE, class STATE_TYPE, class ACTION_TYPE> WordPOSTag SimOneStackWeight<RET_TYPE, STATE_TYPE, ACTION_TYPE>::end_taggedword = WordPOSTag();
	template<class RET_TYPE, class STATE_TYPE, class ACTION_TYPE> TagSet SimOneStackWeight<RET_TYPE, STATE_TYPE, ACTION_TYPE>::empty_tagset = TagSet();

	template<class RET_TYPE, class STATE_TYPE, class ACTION_TYPE>
	SimOneStackWeight<RET_TYPE, STATE_TYPE, ACTION_TYPE>::SimOneStackWeight(const std::string & sRead, const std::string & sRecord,
			DWord * pWords, DPOSTag * pPOSTags, DLabel * pLables, DSuperTag * pSuperTags,
			SuperTagCandidates * wordsCandidates, SuperTagCandidates * postagsCandidates) :
		GraphWeightBase(sRead, sRecord, pWords, pPOSTags, pLables, pSuperTags, wordsCandidates, postagsCandidates),
		// uni-gram
		// st, n0, st2
		m_mapSTw("m_mapSTw"),
		m_mapSTpt("m_mapSTpt"),
		m_mapST2w("m_mapST2w"),
		m_mapST2pt("m_mapST2pt"),
		m_mapN0w("m_mapN0w"),
		m_mapN0pt("m_mapN0pt"),

		// unigram context
		// st + sti, n0 + n0i
		m_mapSTipt("m_mapSTipt"),
		m_mapSTiptSTjpt("m_mapSTiptSTjpt"),
		m_mapN0ipt("m_mapN0ipt"),
		m_mapN0iptN0jpt("m_mapN0iptN0jpt"),

		// bi-gram
		// st + n0
		m_mapSTwN0w("m_mapSTwN0w"),
		m_mapSTwN0pt("m_mapSTwN0pt"),
		m_mapSTptN0w("m_mapSTptN0w"),
		m_mapSTptN0pt("m_mapSTptN0pt"),
		// st + n0 + dis
		m_mapSTptN0ptD("m_mapSTptN0ptD"),
		// st + n0 + st left/right head/pred
		m_mapSTptN0ptSTLPi("m_mapSTptN0ptSTLPi"),
		m_mapSTptN0ptSTRPi("m_mapSTptN0ptSTRPi"),
		m_mapSTptN0ptN0LPi("m_mapSTptN0ptN0LPi"),

		// st2 + n0
		m_mapST2wN0w("m_mapST2wN0w"),
		m_mapST2wN0pt("m_mapST2wN0pt"),
		m_mapST2ptN0w("m_mapST2ptN0w"),
		m_mapST2ptN0pt("m_mapST2ptN0pt"),
		// st2 + n0 + dis
		m_mapST2ptN0ptD("m_mapST2ptN0ptD"),
		// st2 + n0 + st2 left/right head/pred
		m_mapST2ptN0ptST2LPi("m_mapST2wN0wST2LPi"),
		m_mapST2ptN0ptST2RPi("m_mapST2wN0wST2RPi"),
		m_mapST2ptN0ptN0LPi("m_mapST2wN0wN0LPi"),

		// st + st2
		m_mapSTwST2w("m_mapSTwST2w"),
		m_mapSTwST2pt("m_mapSTwST2pt"),
		m_mapSTptST2w("m_mapSTptST2w"),
		m_mapSTptST2pt("m_mapSTptST2pt"),

		// tri-gram
		// st + n0 + st2
		m_mapSTptN0ptST2pt("m_mapSTptN0ptST2pt"),

		// st + n0 + st left/right head/pred
		m_mapSTptN0ptSTLHptSTLHl("m_mapSTptN0ptSTLHptSTLHl"),
		m_mapSTptN0ptSTLPptSTLPl("m_mapSTptN0ptSTLPptSTLPl"),
		m_mapSTptN0ptSTRHptSTRHl("m_mapSTptN0ptSTRHptSTRHl"),
		m_mapSTptN0ptSTRPptSTRPl("m_mapSTptN0ptSTRPptSTRPl"),

		// st + n0 + n0 left head/pred
		m_mapSTptN0ptN0LHptN0LHl("m_mapSTptN0ptN0LHptN0LHl"),
		m_mapSTptN0ptN0LPptN0LPl("m_mapSTptN0ptN0LPptN0LPl"),

		// quar-gram
		// st + n0 + st right head + st right pred
		m_mapSTptN0ptSTRHptSTRPptSTRHlSTRPl("m_mapSTptN0ptSTRHptSTRPptSTRHlSTRPl"),
		// st + n0 + st left pred + st left pred 2
		m_mapSTptN0ptSTLPptSTLP2ptSTLPlSTLP2l("m_mapSTptN0ptSTLPptSTLP2ptSTLPlSTLP2l"),
		// st + n0 + st right pred + st right pred 2
		m_mapSTptN0ptSTRPptSTRP2ptSTRPlSTRP2l("m_mapSTptN0ptSTRPptSTRP2ptSTRPlSTRP2l"),
		// st + n0 + n0 left head + n0 left pred
		m_mapSTptN0ptN0LHptN0LPptN0LHlN0LPl("m_mapSTptN0ptN0LHptN0LPptN0LHlN0LPl"),
		// st + n0 + n0 left pred + n0 left pred 2
		m_mapSTptN0ptN0LPptN0LP2ptN0LPlN0LP2l("m_mapSTptN0ptN0LPptN0LP2ptN0LPlN0LP2l"),

		// st2 + n0 + st2 left/right head/pred
		m_mapST2ptN0ptST2LHptST2LHl("m_mapST2ptN0ptST2LHptST2LHl"),
		m_mapST2ptN0ptST2LPptST2LPl("m_mapST2ptN0ptST2LPptST2LPl"),
		m_mapST2ptN0ptST2RHptST2RHl("m_mapST2ptN0ptST2RHptST2RHl"),
		m_mapST2ptN0ptST2RPptST2RPl("m_mapST2ptN0ptST2RPptST2RPl"),

		// st + n0 + n0 left head/pred
		m_mapST2ptN0ptN0LHptN0LHl("m_mapST2ptN0ptN0LHptN0LHl"),
		m_mapST2ptN0ptN0LPptN0LPl("m_mapST2ptN0ptN0LPptN0LPl"),

		// quar-gram
		// st2 + n0 + st2 right head + st2 right pred
		m_mapST2ptN0ptST2RHptST2RPptST2RHlST2RPl("m_mapST2ptN0ptST2RHptST2RPptST2RHlST2RPl"),
		// st2 + n0 + st2 left pred + st2 left pred 2
		m_mapST2ptN0ptST2LPptST2LP2ptST2LPlST2LP2l("m_mapST2ptN0ptST2LPptST2LP2ptST2LPlST2LP2l"),
		// st2 + n0 + st2 right pred + st2 right pred 2
		m_mapST2ptN0ptST2RPptST2RP2ptST2RPlST2RP2l("m_mapST2ptN0ptST2RPptST2RP2ptST2RPlST2RP2l"),
		// st2 + n0 + n0 left head + n0 left pred
		m_mapST2ptN0ptN0LHptN0LPptN0LHlN0LPl("m_mapST2ptN0ptN0LHptN0LPptN0LHlN0LPl"),
		// st2 + n0 + n0 left pred + n0 left pred 2
		m_mapST2ptN0ptN0LPptN0LP2ptN0LPlN0LP2l("m_mapST2ptN0ptN0LPptN0LP2ptN0LPlN0LP2l"),

		// st + n0 + label set
		m_mapSTptN0ptSTll("m_mapSTptN0ptSTll"),
		m_mapSTptN0ptSTrl("m_mapSTptN0ptSTrl"),
		m_mapSTptN0ptN0ll("m_mapSTptN0ptN0ll"),
		// st2 + n0 + label set
		m_mapST2ptN0ptST2ll("m_mapST2ptN0ptST2ll"),
		m_mapST2ptN0ptST2rl("m_mapST2ptN0ptST2rl"),
		m_mapST2ptN0ptN0ll("m_mapST2ptN0ptN0ll"),

		// char feature (for chinese)
		// st context char
		m_map1CharBeforeST("m_map1CharBeforeST"),
		m_map2CharBeforeST("m_map2CharBeforeST"),
		m_map3CharBeforeST("m_map3CharBeforeST"),
		m_map1CharAfterST("m_map1CharAfterST"),
		m_map2CharAfterST("m_map2CharAfterST"),
		m_map3CharAfterST("m_map3CharAfterST"),
		// n0 context char
		m_map1CharBeforeN0("m_map1CharBeforeN0"),
		m_map2CharBeforeN0("m_map2CharBeforeN0"),
		m_map3CharBeforeN0("m_map3CharBeforeN0"),
		m_map1CharAfterN0("m_map1CharAfterN0"),
		m_map2CharAfterN0("m_map2CharAfterN0"),
		m_map3CharAfterN0("m_map3CharAfterN0"),

		// path feature
		// st - n0
		m_mapSTPOSPath("m_mapSTPOSPath"),
		m_mapSTFPOSPath("m_mapSTFPOSPath"),
		m_mapSTwN0wSTsynhpt("m_mapSTwN0wSTsynhpt"),
		m_mapSTwN0wN0synhpt("m_mapSTwN0wN0synhpt"),
		// st2 - n0
		m_mapST2POSPath("m_mapST2POSPath"),
		m_mapST2FPOSPath("m_mapST2FPOSPath"),
		m_mapST2wN0wST2synhpt("m_mapST2wN0wST2synhpt"),
		m_mapST2wN0wN0synhpt("m_mapST2wN0wN0synhpt"),

		// supertag feature
		m_mapSTst("m_mapSTst"),
		m_mapST2st("m_mapST2st"),
		m_mapN0ist("m_mapN0ist"),
		m_mapSTstST2st("m_mapSTstST2st"),
		m_mapSTstN0w("m_mapSTstN0w"),
		m_mapSTstN0pt("m_mapSTstN0pt"),
		m_mapSTstN0wpt("m_mapSTstN0wpt")
	{

		empty_taggedword.refer(m_pWords->lookup(EMPTY_WORD), m_pPOSTags->lookup(EMPTY_POSTAG));
		start_taggedword.refer(m_pWords->lookup(START_WORD), m_pPOSTags->lookup(START_POSTAG));
		end_taggedword.refer(m_pWords->lookup(END_WORD), m_pPOSTags->lookup(END_POSTAG));

		loadScores();
	}

	template<class RET_TYPE, class STATE_TYPE, class ACTION_TYPE>
	SimOneStackWeight<RET_TYPE, STATE_TYPE, ACTION_TYPE>::~SimOneStackWeight() = default;

	template<class RET_TYPE, class STATE_TYPE, class ACTION_TYPE>
	void SimOneStackWeight<RET_TYPE, STATE_TYPE, ACTION_TYPE>::loadScores() {

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

		input >> *m_pSuperTags;

		input >> *m_pSuperTagCandidatesOfWords;

		input >> *m_pSuperTagCandidatesOfWords;

		// uni-gram
		// st, n0, st2
		input >> m_mapSTw;
		input >> m_mapSTpt;
		input >> m_mapST2w;
		input >> m_mapST2pt;
		input >> m_mapN0w;
		input >> m_mapN0pt;

		// unigram context
		// st + sti, n0 + n0i
		input >> m_mapSTipt;
		input >> m_mapSTiptSTjpt;
		input >> m_mapN0ipt;
		input >> m_mapN0iptN0jpt;

		// bi-gram
		// st + n0
		input >> m_mapSTwN0w;
		input >> m_mapSTwN0pt;
		input >> m_mapSTptN0w;
		input >> m_mapSTptN0pt;
		// st + n0 + dis
		input >> m_mapSTptN0ptD;
		// st + n0 + st left/right head/pred
		input >> m_mapSTptN0ptSTLPi;
		input >> m_mapSTptN0ptSTRPi;
		input >> m_mapSTptN0ptN0LPi;

		// st2 + n0
		input >> m_mapST2wN0w;
		input >> m_mapST2wN0pt;
		input >> m_mapST2ptN0w;
		input >> m_mapST2ptN0pt;
		// st2 + n0 + dis
		input >> m_mapST2ptN0ptD;
		// st2 + n0 + st2 left/right head/pred
		input >> m_mapST2ptN0ptST2LPi;
		input >> m_mapST2ptN0ptST2RPi;
		input >> m_mapST2ptN0ptN0LPi;

		// st + st2
		input >> m_mapSTwST2w;
		input >> m_mapSTwST2pt;
		input >> m_mapSTptST2w;
		input >> m_mapSTptST2pt;

		// tri-gram
		// st + n0 + st2
		input >> m_mapSTptN0ptST2pt;

		// st + n0 + st left/right head/pred
		input >> m_mapSTptN0ptSTLHptSTLHl;
		input >> m_mapSTptN0ptSTLPptSTLPl;
		input >> m_mapSTptN0ptSTRHptSTRHl;
		input >> m_mapSTptN0ptSTRPptSTRPl;

		// st + n0 + n0 left head/pred
		input >> m_mapSTptN0ptN0LHptN0LHl;
		input >> m_mapSTptN0ptN0LPptN0LPl;

		// quar-gram
		// st + n0 + st right head + st right pred
		input >> m_mapSTptN0ptSTRHptSTRPptSTRHlSTRPl;
		// st + n0 + st left pred + st left pred 2
		input >> m_mapSTptN0ptSTLPptSTLP2ptSTLPlSTLP2l;
		// st + n0 + st right pred + st right pred 2
		input >> m_mapSTptN0ptSTRPptSTRP2ptSTRPlSTRP2l;
		// st + n0 + n0 left head + n0 left pred
		input >> m_mapSTptN0ptN0LHptN0LPptN0LHlN0LPl;
		// st + n0 + n0 left pred + n0 left pred 2
		input >> m_mapSTptN0ptN0LPptN0LP2ptN0LPlN0LP2l;

		// st2 + n0 + st2 left/right head/pred
		input >> m_mapST2ptN0ptST2LHptST2LHl;
		input >> m_mapST2ptN0ptST2LPptST2LPl;
		input >> m_mapST2ptN0ptST2RHptST2RHl;
		input >> m_mapST2ptN0ptST2RPptST2RPl;

		// st + n0 + n0 left head/pred
		input >> m_mapST2ptN0ptN0LHptN0LHl;
		input >> m_mapST2ptN0ptN0LPptN0LPl;

		// quar-gram
		// st2 + n0 + st2 right head + st2 right pred
		input >> m_mapST2ptN0ptST2RHptST2RPptST2RHlST2RPl;
		// st2 + n0 + st2 left pred + st2 left pred 2
		input >> m_mapST2ptN0ptST2LPptST2LP2ptST2LPlST2LP2l;
		// st2 + n0 + st2 right pred + st2 right pred 2
		input >> m_mapST2ptN0ptST2RPptST2RP2ptST2RPlST2RP2l;
		// st2 + n0 + n0 left head + n0 left pred
		input >> m_mapST2ptN0ptN0LHptN0LPptN0LHlN0LPl;
		// st2 + n0 + n0 left pred + n0 left pred 2
		input >> m_mapST2ptN0ptN0LPptN0LP2ptN0LPlN0LP2l;

		// st + n0 + label set
		input >> m_mapSTptN0ptSTll;
		input >> m_mapSTptN0ptSTrl;
		input >> m_mapSTptN0ptN0ll;
		// st2 + n0 + label set
		input >> m_mapST2ptN0ptST2ll;
		input >> m_mapST2ptN0ptST2rl;
		input >> m_mapST2ptN0ptN0ll;

		// char feature (for chinese)
		// st context char
		input >> m_map1CharBeforeST;
		input >> m_map2CharBeforeST;
		input >> m_map3CharBeforeST;
		input >> m_map1CharAfterST;
		input >> m_map2CharAfterST;
		input >> m_map3CharAfterST;
		// n0 context char
		input >> m_map1CharBeforeN0;
		input >> m_map2CharBeforeN0;
		input >> m_map3CharBeforeN0;
		input >> m_map1CharAfterN0;
		input >> m_map2CharAfterN0;
		input >> m_map3CharAfterN0;

		// path feature
		// st - n0
		input >> m_mapSTPOSPath;
		input >> m_mapSTFPOSPath;
		input >> m_mapSTwN0wSTsynhpt;
		input >> m_mapSTwN0wN0synhpt;
		// st2 - n0
		input >> m_mapST2POSPath;
		input >> m_mapST2FPOSPath;
		input >> m_mapST2wN0wST2synhpt;
		input >> m_mapST2wN0wN0synhpt;

		// supertag feature
		input >> m_mapSTst;
		input >> m_mapST2st;
		input >> m_mapN0ist;
		input >> m_mapSTstST2st;
		input >> m_mapSTstN0w;
		input >> m_mapSTstN0pt;
		input >> m_mapSTstN0wpt;

		input.close();
	}

	template<class RET_TYPE, class STATE_TYPE, class ACTION_TYPE>
	void SimOneStackWeight<RET_TYPE, STATE_TYPE, ACTION_TYPE>::saveScores() const {

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

		output << *m_pSuperTags;

		output << *m_pSuperTagCandidatesOfWords;

		output << *m_pSuperTagCandidatesOfWords;

		// uni-gram
		// st, n0, st2
		output << m_mapSTw;
		output << m_mapSTpt;
		output << m_mapST2w;
		output << m_mapST2pt;
		output << m_mapN0w;
		output << m_mapN0pt;

		// unigram context
		// st + sti, n0 + n0i
		output << m_mapSTipt;
		output << m_mapSTiptSTjpt;
		output << m_mapN0ipt;
		output << m_mapN0iptN0jpt;

		// bi-gram
		// st + n0
		output << m_mapSTwN0w;
		output << m_mapSTwN0pt;
		output << m_mapSTptN0w;
		output << m_mapSTptN0pt;
		// st + n0 + dis
		output << m_mapSTptN0ptD;
		// st + n0 + st left/right head/pred
		output << m_mapSTptN0ptSTLPi;
		output << m_mapSTptN0ptSTRPi;
		output << m_mapSTptN0ptN0LPi;

		// st2 + n0
		output << m_mapST2wN0w;
		output << m_mapST2wN0pt;
		output << m_mapST2ptN0w;
		output << m_mapST2ptN0pt;
		// st2 + n0 + dis
		output << m_mapST2ptN0ptD;
		// st2 + n0 + st2 left/right head/pred
		output << m_mapST2ptN0ptST2LPi;
		output << m_mapST2ptN0ptST2RPi;
		output << m_mapST2ptN0ptN0LPi;

		// st + st2
		output << m_mapSTwST2w;
		output << m_mapSTwST2pt;
		output << m_mapSTptST2w;
		output << m_mapSTptST2pt;

		// tri-gram
		// st + n0 + st2
		output << m_mapSTptN0ptST2pt;

		// st + n0 + st left/right head/pred
		output << m_mapSTptN0ptSTLHptSTLHl;
		output << m_mapSTptN0ptSTLPptSTLPl;
		output << m_mapSTptN0ptSTRHptSTRHl;
		output << m_mapSTptN0ptSTRPptSTRPl;

		// st + n0 + n0 left head/pred
		output << m_mapSTptN0ptN0LHptN0LHl;
		output << m_mapSTptN0ptN0LPptN0LPl;

		// quar-gram
		// st + n0 + st right head + st right pred
		output << m_mapSTptN0ptSTRHptSTRPptSTRHlSTRPl;
		// st + n0 + st left pred + st left pred 2
		output << m_mapSTptN0ptSTLPptSTLP2ptSTLPlSTLP2l;
		// st + n0 + st right pred + st right pred 2
		output << m_mapSTptN0ptSTRPptSTRP2ptSTRPlSTRP2l;
		// st + n0 + n0 left head + n0 left pred
		output << m_mapSTptN0ptN0LHptN0LPptN0LHlN0LPl;
		// st + n0 + n0 left pred + n0 left pred 2
		output << m_mapSTptN0ptN0LPptN0LP2ptN0LPlN0LP2l;

		// st2 + n0 + st2 left/right head/pred
		output << m_mapST2ptN0ptST2LHptST2LHl;
		output << m_mapST2ptN0ptST2LPptST2LPl;
		output << m_mapST2ptN0ptST2RHptST2RHl;
		output << m_mapST2ptN0ptST2RPptST2RPl;

		// st + n0 + n0 left head/pred
		output << m_mapST2ptN0ptN0LHptN0LHl;
		output << m_mapST2ptN0ptN0LPptN0LPl;

		// quar-gram
		// st2 + n0 + st2 right head + st2 right pred
		output << m_mapST2ptN0ptST2RHptST2RPptST2RHlST2RPl;
		// st2 + n0 + st2 left pred + st2 left pred 2
		output << m_mapST2ptN0ptST2LPptST2LP2ptST2LPlST2LP2l;
		// st2 + n0 + st2 right pred + st2 right pred 2
		output << m_mapST2ptN0ptST2RPptST2RP2ptST2RPlST2RP2l;
		// st2 + n0 + n0 left head + n0 left pred
		output << m_mapST2ptN0ptN0LHptN0LPptN0LHlN0LPl;
		// st2 + n0 + n0 left pred + n0 left pred 2
		output << m_mapST2ptN0ptN0LPptN0LP2ptN0LPlN0LP2l;

		// st + n0 + label set
		output << m_mapSTptN0ptSTll;
		output << m_mapSTptN0ptSTrl;
		output << m_mapSTptN0ptN0ll;
		// st2 + n0 + label set
		output << m_mapST2ptN0ptST2ll;
		output << m_mapST2ptN0ptST2rl;
		output << m_mapST2ptN0ptN0ll;

		// char feature (for chinese)
		// st context char
		output << m_map1CharBeforeST;
		output << m_map2CharBeforeST;
		output << m_map3CharBeforeST;
		output << m_map1CharAfterST;
		output << m_map2CharAfterST;
		output << m_map3CharAfterST;
		// n0 context char
		output << m_map1CharBeforeN0;
		output << m_map2CharBeforeN0;
		output << m_map3CharBeforeN0;
		output << m_map1CharAfterN0;
		output << m_map2CharAfterN0;
		output << m_map3CharAfterN0;

		// path feature
		// st - n0
		output << m_mapSTPOSPath;
		output << m_mapSTFPOSPath;
		output << m_mapSTwN0wSTsynhpt;
		output << m_mapSTwN0wN0synhpt;
		// st2 - n0
		output << m_mapST2POSPath;
		output << m_mapST2FPOSPath;
		output << m_mapST2wN0wST2synhpt;
		output << m_mapST2wN0wN0synhpt;

		// supertag feature
		output << m_mapSTst;
		output << m_mapST2st;
		output << m_mapN0ist;
		output << m_mapSTstST2st;
		output << m_mapSTstN0w;
		output << m_mapSTstN0pt;
		output << m_mapSTstN0wpt;

		output.close();
	}

	template<class RET_TYPE, class STATE_TYPE, class ACTION_TYPE>
	void SimOneStackWeight<RET_TYPE, STATE_TYPE, ACTION_TYPE>::computeAverageFeatureWeights(const int & round) {

		// uni-gram
		// st, n0, st2
		m_mapSTw.computeAverage(round);
		m_mapSTpt.computeAverage(round);
		m_mapST2w.computeAverage(round);
		m_mapST2pt.computeAverage(round);
		m_mapN0w.computeAverage(round);
		m_mapN0pt.computeAverage(round);

		// unigram context
		// st + sti, n0 + n0i
		m_mapSTipt.computeAverage(round);
		m_mapSTiptSTjpt.computeAverage(round);
		m_mapN0ipt.computeAverage(round);
		m_mapN0iptN0jpt.computeAverage(round);

		// bi-gram
		// st + n0
		m_mapSTwN0w.computeAverage(round);
		m_mapSTwN0pt.computeAverage(round);
		m_mapSTptN0w.computeAverage(round);
		m_mapSTptN0pt.computeAverage(round);
		// st + n0 + dis
		m_mapSTptN0ptD.computeAverage(round);
		// st + n0 + st left/right head/pred
		m_mapSTptN0ptSTLPi.computeAverage(round);
		m_mapSTptN0ptSTRPi.computeAverage(round);
		m_mapSTptN0ptN0LPi.computeAverage(round);

		// st2 + n0
		m_mapST2wN0w.computeAverage(round);
		m_mapST2wN0pt.computeAverage(round);
		m_mapST2ptN0w.computeAverage(round);
		m_mapST2ptN0pt.computeAverage(round);
		// st2 + n0 + dis
		m_mapST2ptN0ptD.computeAverage(round);
		// st2 + n0 + st2 left/right head/pred
		m_mapST2ptN0ptST2LPi.computeAverage(round);
		m_mapST2ptN0ptST2RPi.computeAverage(round);
		m_mapST2ptN0ptN0LPi.computeAverage(round);

		// st + st2
		m_mapSTwST2w.computeAverage(round);
		m_mapSTwST2pt.computeAverage(round);
		m_mapSTptST2w.computeAverage(round);
		m_mapSTptST2pt.computeAverage(round);

		// tri-gram
		// st + n0 + st2
		m_mapSTptN0ptST2pt.computeAverage(round);

		// st + n0 + st left/right head/pred
		m_mapSTptN0ptSTLHptSTLHl.computeAverage(round);
		m_mapSTptN0ptSTLPptSTLPl.computeAverage(round);
		m_mapSTptN0ptSTRHptSTRHl.computeAverage(round);
		m_mapSTptN0ptSTRPptSTRPl.computeAverage(round);

		// st + n0 + n0 left head/pred
		m_mapSTptN0ptN0LHptN0LHl.computeAverage(round);
		m_mapSTptN0ptN0LPptN0LPl.computeAverage(round);

		// quar-gram
		// st + n0 + st right head + st right pred
		m_mapSTptN0ptSTRHptSTRPptSTRHlSTRPl.computeAverage(round);
		// st + n0 + st left pred + st left pred 2
		m_mapSTptN0ptSTLPptSTLP2ptSTLPlSTLP2l.computeAverage(round);
		// st + n0 + st right pred + st right pred 2
		m_mapSTptN0ptSTRPptSTRP2ptSTRPlSTRP2l.computeAverage(round);
		// st + n0 + n0 left head + n0 left pred
		m_mapSTptN0ptN0LHptN0LPptN0LHlN0LPl.computeAverage(round);
		// st + n0 + n0 left pred + n0 left pred 2
		m_mapSTptN0ptN0LPptN0LP2ptN0LPlN0LP2l.computeAverage(round);

		// st2 + n0 + st2 left/right head/pred
		m_mapST2ptN0ptST2LHptST2LHl.computeAverage(round);
		m_mapST2ptN0ptST2LPptST2LPl.computeAverage(round);
		m_mapST2ptN0ptST2RHptST2RHl.computeAverage(round);
		m_mapST2ptN0ptST2RPptST2RPl.computeAverage(round);

		// st + n0 + n0 left head/pred
		m_mapST2ptN0ptN0LHptN0LHl.computeAverage(round);
		m_mapST2ptN0ptN0LPptN0LPl.computeAverage(round);

		// quar-gram
		// st2 + n0 + st2 right head + st2 right pred
		m_mapST2ptN0ptST2RHptST2RPptST2RHlST2RPl.computeAverage(round);
		// st2 + n0 + st2 left pred + st2 left pred 2
		m_mapST2ptN0ptST2LPptST2LP2ptST2LPlST2LP2l.computeAverage(round);
		// st2 + n0 + st2 right pred + st2 right pred 2
		m_mapST2ptN0ptST2RPptST2RP2ptST2RPlST2RP2l.computeAverage(round);
		// st2 + n0 + n0 left head + n0 left pred
		m_mapST2ptN0ptN0LHptN0LPptN0LHlN0LPl.computeAverage(round);
		// st2 + n0 + n0 left pred + n0 left pred 2
		m_mapST2ptN0ptN0LPptN0LP2ptN0LPlN0LP2l.computeAverage(round);

		// st + n0 + label set
		m_mapSTptN0ptSTll.computeAverage(round);
		m_mapSTptN0ptSTrl.computeAverage(round);
		m_mapSTptN0ptN0ll.computeAverage(round);
		// st2 + n0 + label set
		m_mapST2ptN0ptST2ll.computeAverage(round);
		m_mapST2ptN0ptST2rl.computeAverage(round);
		m_mapST2ptN0ptN0ll.computeAverage(round);

		// char feature (for chinese)
		// st context char
		m_map1CharBeforeST.computeAverage(round);
		m_map2CharBeforeST.computeAverage(round);
		m_map3CharBeforeST.computeAverage(round);
		m_map1CharAfterST.computeAverage(round);
		m_map2CharAfterST.computeAverage(round);
		m_map3CharAfterST.computeAverage(round);
		// n0 context char
		m_map1CharBeforeN0.computeAverage(round);
		m_map2CharBeforeN0.computeAverage(round);
		m_map3CharBeforeN0.computeAverage(round);
		m_map1CharAfterN0.computeAverage(round);
		m_map2CharAfterN0.computeAverage(round);
		m_map3CharAfterN0.computeAverage(round);

		// path feature
		// st - n0
		m_mapSTPOSPath.computeAverage(round);
		m_mapSTFPOSPath.computeAverage(round);
		m_mapSTwN0wSTsynhpt.computeAverage(round);
		m_mapSTwN0wN0synhpt.computeAverage(round);
		// st2 - n0
		m_mapST2POSPath.computeAverage(round);
		m_mapST2FPOSPath.computeAverage(round);
		m_mapST2wN0wST2synhpt.computeAverage(round);
		m_mapST2wN0wN0synhpt.computeAverage(round);

		// supertag feature
		m_mapSTst.computeAverage(round);
		m_mapST2st.computeAverage(round);
		m_mapN0ist.computeAverage(round);
		m_mapSTstST2st.computeAverage(round);
		m_mapSTstN0w.computeAverage(round);
		m_mapSTstN0pt.computeAverage(round);
		m_mapSTstN0wpt.computeAverage(round);
	}

	template<class RET_TYPE, class STATE_TYPE, class ACTION_TYPE>
	void SimOneStackWeight<RET_TYPE, STATE_TYPE, ACTION_TYPE>::getOrUpdateFeatureScores(GraphDepParserBase<RET_TYPE, STATE_TYPE, ACTION_TYPE> * parser, const STATE_TYPE & item, const ActionScoreIncrement & amount) {
		int dis = -1;

		const int outIndex = -1;
		const int emptyLabel = 0;
		const int emptyArity = -1;
		const int emptySuperTag = 0;

		WordPOSTag (&sentence)[MAX_SENTENCE_SIZE] = parser->m_lSentence;
		RET_TYPE & packedScore = parser->m_lPackedScore;
		const int & scoreIndex = parser->m_nScoreIndex;
		const int & trainingRound = parser->m_nTrainingRound;
		const int & sentenceLength = parser->m_nSentenceLength;

		// st
		const int & st_index = item.stackBack() == -1 ? outIndex : item.stackTop();
		const int & stlh_index = st_index == -1 ? outIndex : item.leftHead(st_index);
		const int & stlp_index = st_index == -1 ? outIndex : item.leftPred(st_index);
		const int & strh_index = st_index == -1 ? outIndex : item.rightHead(st_index);
		const int & strp_index = st_index == -1 ? outIndex : item.rightPred(st_index);
		// st
		const WordPOSTag & st_word_postag = st_index == -1 ? start_taggedword : sentence[st_index];
		const WordPOSTag & stlh_word_postag = stlh_index == -1 ? empty_taggedword : sentence[stlh_index];
		const WordPOSTag & stlp_word_postag = stlp_index == -1 ? empty_taggedword : sentence[stlp_index];
		const WordPOSTag & strh_word_postag = strh_index == -1 ? empty_taggedword : sentence[strh_index];
		const WordPOSTag & strp_word_postag = strp_index == -1 ? empty_taggedword : sentence[strp_index];
		// st
		const Word & st_word = st_word_postag.first();
		const POSTag & st_postag = st_word_postag.second();
		const TagSet & st_llabelset = st_index == -1 ? empty_tagset : item.leftPredLabelSet(st_index);
		const TagSet & st_rlabelset = st_index == -1 ? empty_tagset : item.rightPredLabelSet(st_index);
		const POSTag & stlh_postag = stlh_word_postag.second();
		const int & stlh_label = st_index == -1 ? emptyLabel : item.leftHeadLabel(st_index);
		const POSTag & stlp_postag = stlp_word_postag.second();
		const POSTag & stlp2_postag = (st_index == -1 || item.leftSubPred(st_index) == -1) ? empty_taggedword.second() : sentence[item.leftSubPred(st_index)].second();
		const int & stlp_label = st_index == -1 ? emptyLabel : item.leftPredLabel(st_index);
		const int & stlp2_label = st_index == -1 ? emptyLabel : item.leftSubPredLabel(st_index);
		const POSTag & strh_postag = strh_word_postag.second();
		const int & strh_label = st_index == -1 ? emptyLabel : item.rightHeadLabel(st_index);
		const POSTag & strp_postag = strp_word_postag.second();
		const POSTag & strp2_postag = (st_index == -1 || item.rightSubPred(st_index) == -1) ? empty_taggedword.second() : sentence[item.rightSubPred(st_index)].second();
		const int & strp_label = st_index == -1 ? emptyLabel : item.rightPredLabel(st_index);
		const int & strp2_label = st_index == -1 ? emptyLabel : item.rightSubPredLabel(st_index);
		const int & strp_arity = st_index == -1 ? emptyArity : item.rightPredArity(st_index);

		// st2
		const int & st2_index = item.stackBack() <= 0 ? outIndex : item.stackSubTop();
		const int & st2lh_index = st2_index == -1 ? outIndex : item.leftHead(st2_index);
		const int & st2lp_index = st2_index == -1 ? outIndex : item.leftPred(st2_index);
		const int & st2rh_index = st2_index == -1 ? outIndex : item.rightHead(st2_index);
		const int & st2rp_index = st2_index == -1 ? outIndex : item.rightPred(st2_index);
		// st2
		const WordPOSTag & st2_word_postag = st2_index == -1 ? start_taggedword : sentence[st2_index];
		const WordPOSTag & st2lh_word_postag = st2lh_index == -1 ? empty_taggedword : sentence[st2lh_index];
		const WordPOSTag & st2lp_word_postag = st2lp_index == -1 ? empty_taggedword : sentence[st2lp_index];
		const WordPOSTag & st2rh_word_postag = st2rh_index == -1 ? empty_taggedword : sentence[st2rh_index];
		const WordPOSTag & st2rp_word_postag = st2rp_index == -1 ? empty_taggedword : sentence[st2rp_index];
		// st2
		const Word & st2_word = st2_word_postag.first();
		const POSTag & st2_postag = st2_word_postag.second();
		const TagSet & st2_llabelset = st2_index == -1 ? empty_tagset : item.leftPredLabelSet(st2_index);
		const TagSet & st2_rlabelset = st2_index == -1 ? empty_tagset : item.rightPredLabelSet(st2_index);
		const POSTag & st2lh_postag = st2lh_word_postag.second();
		const int & st2lh_label = st2_index == -1 ? emptyLabel : item.leftHeadLabel(st2_index);
		const POSTag & st2lp_postag = st2lp_word_postag.second();
		const POSTag & st2lp2_postag = (st2_index == -1 || item.leftSubPred(st2_index) == -1) ? empty_taggedword.second() : sentence[item.leftSubPred(st2_index)].second();
		const int & st2lp_label = st2_index == -1 ? emptyLabel : item.leftPredLabel(st2_index);
		const int & st2lp2_label = st2_index == -1 ? emptyLabel : item.leftSubPredLabel(st2_index);
		const POSTag & st2rh_postag = st2rh_word_postag.second();
		const int & st2rh_label = st2_index == -1 ? emptyLabel : item.rightHeadLabel(st2_index);
		const POSTag & st2rp_postag = st2rp_word_postag.second();
		const POSTag & st2rp2_postag = (st2_index == -1 || item.rightSubPred(st2_index) == -1) ? empty_taggedword.second() : sentence[item.rightSubPred(st2_index)].second();
		const int & st2rp_label = st2_index == -1 ? emptyLabel : item.rightPredLabel(st2_index);
		const int & st2rp2_label = st2_index == -1 ? emptyLabel : item.rightSubPredLabel(st2_index);
		const int & st2rp_arity = st2_index == -1 ? emptyArity : item.rightPredArity(st2_index);

		// n0
		const int & n0_index = item.size() < sentenceLength ? item.size() : outIndex;
		const int & n0lp_index = n0_index == -1 ? outIndex : item.leftPred(n0_index);
		const int & n0lh_index = n0_index == -1 ? outIndex : item.leftHead(n0_index);
		// n0
		const WordPOSTag & n0_word_postag = n0_index == -1 ? end_taggedword : sentence[n0_index];
		const WordPOSTag & n0lh_word_postag = n0lh_index == -1 ? empty_taggedword : sentence[n0lh_index];
		const WordPOSTag & n0lp_word_postag = n0lp_index == -1 ? empty_taggedword : sentence[n0lp_index];
		// n0
		const Word & n0_word = n0_word_postag.first();
		const POSTag & n0_postag = n0_word_postag.second();
		const TagSet & n0_llabelset = n0_index == -1 ? empty_tagset : item.leftPredLabelSet(n0_index);
		const POSTag & n0lh_postag = n0lh_word_postag.second();
		const int & n0lh_label = n0_index == -1 ? emptyLabel : item.leftHeadLabel(n0_index);
		const POSTag & n0lp_postag = n0lp_word_postag.second();
		const POSTag & n0lp2_postag = (n0_index == -1 || item.leftSubPred(n0_index) == -1) ? empty_taggedword.second() : sentence[item.leftSubPred(n0_index)].second();
		const int & n0lp_label = n0_index == -1 ? emptyLabel : item.leftPredLabel(n0_index);
		const int & n0lp2_label = n0_index == -1 ? emptyLabel : item.leftSubPredLabel(n0_index);
		const int & n0lp_arity = n0_index == -1 ? emptyArity : item.leftPredArity(n0_index);

		// n0
		m_mapN0w.getOrUpdateScore(packedScore, n0_word, scoreIndex, amount, trainingRound);
		m_mapN0pt.getOrUpdateScore(packedScore, n0_postag, scoreIndex, amount, trainingRound);

		// st
		m_mapSTw.getOrUpdateScore(packedScore, st_word, scoreIndex, amount, trainingRound);
		m_mapSTpt.getOrUpdateScore(packedScore, st_postag, scoreIndex, amount, trainingRound);
		// bigram st + n0
		poses_feature = ENCODE_POSTAG_SET_2(st_postag, n0_postag);
		m_mapSTptN0pt.getOrUpdateScore(packedScore, poses_feature, scoreIndex, amount, trainingRound);
		bi_features.refer(st_word, n0_word);
		m_mapSTwN0w.getOrUpdateScore(packedScore, bi_features, scoreIndex, amount, trainingRound);
		bi_features.referLast(n0_postag);
		m_mapSTwN0pt.getOrUpdateScore(packedScore, bi_features, scoreIndex, amount, trainingRound);
		bi_features.refer(n0_word, st_postag);
		m_mapSTptN0w.getOrUpdateScore(packedScore, bi_features, scoreIndex, amount, trainingRound);
		// bigram with arity
		bi_features.refer(poses_feature, strp_arity);
		m_mapSTptN0ptSTRPi.getOrUpdateScore(packedScore, bi_features, scoreIndex, amount, trainingRound);
		bi_features.referLast(n0lp_arity);
		m_mapSTptN0ptN0LPi.getOrUpdateScore(packedScore, bi_features, scoreIndex, amount, trainingRound);
		// bigram with dis
		dis = encodeLinkDistance(st_index, n0_index == -1 ? sentenceLength : n0_index);
		bi_features.referLast(dis);
		m_mapSTptN0ptD.getOrUpdateScore(packedScore, bi_features, scoreIndex, amount, trainingRound);

		// st2
		m_mapST2w.getOrUpdateScore(packedScore, st2_word, scoreIndex, amount, trainingRound);
		m_mapST2pt.getOrUpdateScore(packedScore, st2_postag, scoreIndex, amount, trainingRound);
		// bigram st2 + n0
		poses_feature = ENCODE_POSTAG_SET_2(st2_postag, n0_postag);
		m_mapST2ptN0pt.getOrUpdateScore(packedScore, poses_feature, scoreIndex, amount, trainingRound);
		bi_features.refer(st2_word, n0_word);
		m_mapST2wN0w.getOrUpdateScore(packedScore, bi_features, scoreIndex, amount, trainingRound);
		bi_features.referLast(n0_postag);
		m_mapST2wN0pt.getOrUpdateScore(packedScore, bi_features, scoreIndex, amount, trainingRound);
		bi_features.refer(n0_word, st2_postag);
		m_mapST2ptN0w.getOrUpdateScore(packedScore, bi_features, scoreIndex, amount, trainingRound);
		// bigram with arity
		bi_features.refer(poses_feature, st2rp_arity);
		m_mapST2ptN0ptST2RPi.getOrUpdateScore(packedScore, bi_features, scoreIndex, amount, trainingRound);
		bi_features.referLast(n0lp_arity);
		m_mapST2ptN0ptN0LPi.getOrUpdateScore(packedScore, bi_features, scoreIndex, amount, trainingRound);
		// bigram with dis
		dis = encodeLinkDistance(st2_index, n0_index == -1 ? sentenceLength : n0_index);
		bi_features.referLast(dis);
		m_mapST2ptN0ptD.getOrUpdateScore(packedScore, bi_features, scoreIndex, amount, trainingRound);
		// bigram st + st2
		poses_feature = ENCODE_POSTAG_SET_2(st_postag, st2_postag);
		m_mapSTptST2pt.getOrUpdateScore(packedScore, poses_feature, scoreIndex, amount, trainingRound);
		bi_features.refer(st_word, st2_word);
		m_mapSTwST2w.getOrUpdateScore(packedScore, bi_features, scoreIndex, amount, trainingRound);
		bi_features.referLast(st2_postag);
		m_mapSTwST2pt.getOrUpdateScore(packedScore, bi_features, scoreIndex, amount, trainingRound);
		bi_features.refer(st2_word, st_postag);
		m_mapSTptST2w.getOrUpdateScore(packedScore, bi_features, scoreIndex, amount, trainingRound);

		// tri-gram
		// st + n0 + st2
		poses_feature = ENCODE_POSTAG_SET_3(st_postag, st2_postag, n0_postag);
		m_mapSTptN0ptST2pt.getOrUpdateScore(packedScore, poses_feature, scoreIndex, amount, trainingRound);

		// st + n0 + st left/right head/pred
		bi_features.refer(ENCODE_POSTAG_SET_3(st_postag, stlh_postag, n0_postag), stlh_label);
		m_mapSTptN0ptSTLHptSTLHl.getOrUpdateScore(packedScore, bi_features, scoreIndex, amount, trainingRound);
		bi_features.refer(ENCODE_POSTAG_SET_3(st_postag, stlp_postag, n0_postag), stlp_label);
		m_mapSTptN0ptSTLPptSTLPl.getOrUpdateScore(packedScore, bi_features, scoreIndex, amount, trainingRound);
		bi_features.refer(ENCODE_POSTAG_SET_3(st_postag, strh_postag, n0_postag), strh_label);
		m_mapSTptN0ptSTRHptSTRHl.getOrUpdateScore(packedScore, bi_features, scoreIndex, amount, trainingRound);
		bi_features.refer(ENCODE_POSTAG_SET_3(st_postag, strp_postag, n0_postag), strp_label);
		m_mapSTptN0ptSTRPptSTRPl.getOrUpdateScore(packedScore, bi_features, scoreIndex, amount, trainingRound);

		// st + n0 + n0 left head/pred
		bi_features.refer(ENCODE_POSTAG_SET_3(st_postag, n0_postag, n0lh_postag), n0lh_label);
		m_mapSTptN0ptN0LHptN0LHl.getOrUpdateScore(packedScore, bi_features, scoreIndex, amount, trainingRound);
		bi_features.refer(ENCODE_POSTAG_SET_3(st_postag, n0_postag, n0lp_postag), n0lp_label);
		m_mapSTptN0ptN0LPptN0LPl.getOrUpdateScore(packedScore, bi_features, scoreIndex, amount, trainingRound);

		// quar-gram
		// st + n0 + st right head + st right pred
		tri_features.refer(encodePOSTagSet4(st_postag, strh_postag, strp_postag, n0_postag), strh_label, strp_label);
		m_mapSTptN0ptSTRHptSTRPptSTRHlSTRPl.getOrUpdateScore(packedScore, tri_features, scoreIndex, amount, trainingRound);
		// st + n0 + st left pred + st left pred 2
		tri_features.refer(encodePOSTagSet4(st_postag, stlp_postag, stlp2_postag, n0_postag), stlp_label, stlp2_label);
		m_mapSTptN0ptSTLPptSTLP2ptSTLPlSTLP2l.getOrUpdateScore(packedScore, tri_features, scoreIndex, amount, trainingRound);
		// st + n0 + st right pred + st right pred 2
		tri_features.refer(encodePOSTagSet4(st_postag, strp_postag, strp2_postag, n0_postag), strp_label, strp2_label);
		m_mapSTptN0ptSTRPptSTRP2ptSTRPlSTRP2l.getOrUpdateScore(packedScore, tri_features, scoreIndex, amount, trainingRound);
		// st + n0 + n0 left head + n0 left pred
		tri_features.refer(encodePOSTagSet4(st_postag, n0_postag, n0lh_postag, n0lp_postag), n0lh_label, n0lp_label);
		m_mapSTptN0ptN0LHptN0LPptN0LHlN0LPl.getOrUpdateScore(packedScore, tri_features, scoreIndex, amount, trainingRound);
		// st + n0 + n0 left pred + n0 left pred 2
		tri_features.refer(encodePOSTagSet4(st_postag, n0_postag, n0lp_postag, n0lp2_postag), n0lp_label, n0lp2_label);
		m_mapSTptN0ptN0LPptN0LP2ptN0LPlN0LP2l.getOrUpdateScore(packedScore, tri_features, scoreIndex, amount, trainingRound);

		// st2 + n0 + st2 left/right head/pred
		bi_features.refer(ENCODE_POSTAG_SET_3(st2_postag, st2lh_postag, n0_postag), st2lh_label);
		m_mapST2ptN0ptST2LHptST2LHl.getOrUpdateScore(packedScore, bi_features, scoreIndex, amount, trainingRound);
		bi_features.refer(ENCODE_POSTAG_SET_3(st2_postag, st2lp_postag, n0_postag), st2lp_label);
		m_mapST2ptN0ptST2LPptST2LPl.getOrUpdateScore(packedScore, bi_features, scoreIndex, amount, trainingRound);
		bi_features.refer(ENCODE_POSTAG_SET_3(st2_postag, st2rh_postag, n0_postag), st2rh_label);
		m_mapST2ptN0ptST2RHptST2RHl.getOrUpdateScore(packedScore, bi_features, scoreIndex, amount, trainingRound);
		bi_features.refer(ENCODE_POSTAG_SET_3(st2_postag, st2rp_postag, n0_postag), st2rp_label);
		m_mapST2ptN0ptST2RPptST2RPl.getOrUpdateScore(packedScore, bi_features, scoreIndex, amount, trainingRound);

		// st2 + n0 + n0 left head/pred
		bi_features.refer(ENCODE_POSTAG_SET_3(st2_postag, n0_postag, n0lh_postag), n0lh_label);
		m_mapST2ptN0ptN0LHptN0LHl.getOrUpdateScore(packedScore, bi_features, scoreIndex, amount, trainingRound);
		bi_features.refer(ENCODE_POSTAG_SET_3(st2_postag, n0_postag, n0lp_postag), n0lp_label);
		m_mapST2ptN0ptN0LPptN0LPl.getOrUpdateScore(packedScore, bi_features, scoreIndex, amount, trainingRound);

		// quar-gram
		// st2 + n0 + st2 right head + st2 right pred
		tri_features.refer(encodePOSTagSet4(st2_postag, st2rh_postag, st2rp_postag, n0_postag), st2rh_label, st2rp_label);
		m_mapST2ptN0ptST2RHptST2RPptST2RHlST2RPl.getOrUpdateScore(packedScore, tri_features, scoreIndex, amount, trainingRound);
		// st2 + n0 + st2 left pred + st2 left pred 2
		tri_features.refer(encodePOSTagSet4(st2_postag, st2lp_postag, st2lp2_postag, n0_postag), st2lp_label, st2lp2_label);
		m_mapST2ptN0ptST2LPptST2LP2ptST2LPlST2LP2l.getOrUpdateScore(packedScore, tri_features, scoreIndex, amount, trainingRound);
		// st2 + n0 + st2 right pred + st2 right pred 2
		tri_features.refer(encodePOSTagSet4(st2_postag, st2rp_postag, st2rp2_postag, n0_postag), st2rp_label, st2rp2_label);
		m_mapST2ptN0ptST2RPptST2RP2ptST2RPlST2RP2l.getOrUpdateScore(packedScore, tri_features, scoreIndex, amount, trainingRound);
		// st2 + n0 + n0 left head + n0 left pred
		tri_features.refer(encodePOSTagSet4(st2_postag, n0_postag, n0lh_postag, n0lp_postag), n0lh_label, n0lp_label);
		m_mapST2ptN0ptN0LHptN0LPptN0LHlN0LPl.getOrUpdateScore(packedScore, tri_features, scoreIndex, amount, trainingRound);
		// st2 + n0 + n0 left pred + n0 left pred 2
		tri_features.refer(encodePOSTagSet4(st2_postag, n0_postag, n0lp_postag, n0lp2_postag), n0lp_label, n0lp2_label);
		m_mapST2ptN0ptN0LPptN0LP2ptN0LPlN0LP2l.getOrUpdateScore(packedScore, tri_features, scoreIndex, amount, trainingRound);

		// st + n0 + labelset
		poses_feature = ENCODE_POSTAG_SET_2(st_postag, n0_postag);
		uni_tagset.refer(poses_feature, st_llabelset.bits(0), st_llabelset.bits(1));
		m_mapSTptN0ptSTll.getOrUpdateScore(packedScore, uni_tagset, scoreIndex, amount, trainingRound);
		uni_tagset.refer(poses_feature, st_rlabelset.bits(0), st_rlabelset.bits(1));
		m_mapSTptN0ptSTrl.getOrUpdateScore(packedScore, uni_tagset, scoreIndex, amount, trainingRound);
		uni_tagset.refer(poses_feature, n0_llabelset.bits(0), n0_llabelset.bits(1));
		m_mapSTptN0ptN0ll.getOrUpdateScore(packedScore, uni_tagset, scoreIndex, amount, trainingRound);

		// st2 + n0 + labelset
		poses_feature = ENCODE_POSTAG_SET_2(st2_postag, n0_postag);
		uni_tagset.refer(poses_feature, st2_llabelset.bits(0), st2_llabelset.bits(1));
		m_mapST2ptN0ptST2ll.getOrUpdateScore(packedScore, uni_tagset, scoreIndex, amount, trainingRound);
		uni_tagset.refer(poses_feature, st2_rlabelset.bits(0), st2_rlabelset.bits(1));
		m_mapST2ptN0ptST2rl.getOrUpdateScore(packedScore, uni_tagset, scoreIndex, amount, trainingRound);
		uni_tagset.refer(poses_feature, n0_llabelset.bits(0), n0_llabelset.bits(1));
		m_mapST2ptN0ptN0ll.getOrUpdateScore(packedScore, uni_tagset, scoreIndex, amount, trainingRound);

		// context
		const int & stl2_index = st_index >= 2 ? st_index - 2 : outIndex;
		const int & stl1_index = st_index >= 1 ? st_index - 1 : outIndex;
		const int & str1_index = st_index + 1 < sentenceLength ? st_index + 1 : outIndex;
		const int & str2_index = st_index + 2 < sentenceLength ? st_index + 2 : outIndex;
		const int & n0l2_index = n0_index >= 2 ? n0_index - 2 : outIndex;
		const int & n0l1_index = n0_index >= 1 ? n0_index - 1 : outIndex;
		const int & n0r1_index = n0_index + 1 < sentenceLength ? n0_index + 1 : outIndex;
		const int & n0r2_index = n0_index + 2 < sentenceLength ? n0_index + 2 : outIndex;
		// context
		const WordPOSTag & stl2_word_postag = stl2_index == -1 ? start_taggedword : sentence[stl2_index];
		const WordPOSTag & stl1_word_postag = stl1_index == -1 ? start_taggedword : sentence[stl1_index];
		const WordPOSTag & str1_word_postag = str1_index == -1 ? end_taggedword : sentence[str1_index];
		const WordPOSTag & str2_word_postag = str2_index == -1 ? end_taggedword : sentence[str2_index];
		const WordPOSTag & n0l2_word_postag = n0l2_index == -1 ? start_taggedword : sentence[n0l2_index];
		const WordPOSTag & n0l1_word_postag = n0l1_index == -1 ? start_taggedword : sentence[n0l1_index];
		const WordPOSTag & n0r1_word_postag = n0r1_index == -1 ? end_taggedword : sentence[n0r1_index];
		const WordPOSTag & n0r2_word_postag = n0r2_index == -1 ? end_taggedword : sentence[n0r2_index];
		// context
		const POSTag & stl2_postag = stl2_word_postag.second();
		const POSTag & stl1_postag = stl1_word_postag.second();
		const POSTag & str1_postag = str1_word_postag.second();
		const POSTag & str2_postag = str2_word_postag.second();
		const POSTag & n0l2_postag = n0l2_word_postag.second();
		const POSTag & n0l1_postag = n0l1_word_postag.second();
		const POSTag & n0r1_postag = n0r1_word_postag.second();
		const POSTag & n0r2_postag = n0r2_word_postag.second();
		// unigram context
		// st + sti, n0 + n0i
		// st pos
		bi_features.refer(stl2_postag, -2);
		m_mapSTipt.getOrUpdateScore(packedScore, bi_features, scoreIndex, amount, trainingRound);
		bi_features.refer(stl1_postag, -1);
		m_mapSTipt.getOrUpdateScore(packedScore, bi_features, scoreIndex, amount, trainingRound);
		bi_features.refer(str1_postag, 1);
		m_mapSTipt.getOrUpdateScore(packedScore, bi_features, scoreIndex, amount, trainingRound);
		bi_features.refer(str2_postag, 2);
		m_mapSTipt.getOrUpdateScore(packedScore, bi_features, scoreIndex, amount, trainingRound);
		bi_features.refer(ENCODE_POSTAG_SET_2(stl2_postag, stl1_postag), -1);
		m_mapSTiptSTjpt.getOrUpdateScore(packedScore, bi_features, scoreIndex, amount, trainingRound);
		bi_features.refer(ENCODE_POSTAG_SET_2(stl1_postag, str1_postag), 0);
		m_mapSTiptSTjpt.getOrUpdateScore(packedScore, bi_features, scoreIndex, amount, trainingRound);
		bi_features.refer(ENCODE_POSTAG_SET_2(str1_postag, str2_postag), 1);
		m_mapSTiptSTjpt.getOrUpdateScore(packedScore, bi_features, scoreIndex, amount, trainingRound);
		// n0 pos
		bi_features.refer(n0l2_postag, -2);
		m_mapN0ipt.getOrUpdateScore(packedScore, bi_features, scoreIndex, amount, trainingRound);
		bi_features.refer(n0l1_postag, -1);
		m_mapN0ipt.getOrUpdateScore(packedScore, bi_features, scoreIndex, amount, trainingRound);
		bi_features.refer(n0r1_postag, 1);
		m_mapN0ipt.getOrUpdateScore(packedScore, bi_features, scoreIndex, amount, trainingRound);
		bi_features.refer(n0r2_postag, 2);
		m_mapN0ipt.getOrUpdateScore(packedScore, bi_features, scoreIndex, amount, trainingRound);
		bi_features.refer(ENCODE_POSTAG_SET_2(n0l2_postag, n0l1_postag), -1);
		m_mapN0iptN0jpt.getOrUpdateScore(packedScore, bi_features, scoreIndex, amount, trainingRound);
		bi_features.refer(ENCODE_POSTAG_SET_2(n0l1_postag, n0r1_postag), 0);
		m_mapN0iptN0jpt.getOrUpdateScore(packedScore, bi_features, scoreIndex, amount, trainingRound);
		bi_features.refer(ENCODE_POSTAG_SET_2(n0r1_postag, n0r2_postag), 1);
		m_mapN0iptN0jpt.getOrUpdateScore(packedScore, bi_features, scoreIndex, amount, trainingRound);

		if (parser->m_bChar) {
			const Sentence & charSentence = parser->m_sSentence;

			m_map1CharBeforeST.getOrUpdateScore(packedScore, nCharPrev(charSentence, st_index == -1 ? 0 : st_index, 1), scoreIndex, amount, trainingRound);
			m_map2CharBeforeST.getOrUpdateScore(packedScore, nCharPrev(charSentence, st_index == -1 ? 0 : st_index, 2), scoreIndex, amount, trainingRound);
			m_map3CharBeforeST.getOrUpdateScore(packedScore, nCharPrev(charSentence, st_index == -1 ? 0 : st_index, 3), scoreIndex, amount, trainingRound);
			m_map1CharAfterST.getOrUpdateScore(packedScore, nCharNext(charSentence, st_index == -1 ? 0 : st_index, 1), scoreIndex, amount, trainingRound);
			m_map2CharAfterST.getOrUpdateScore(packedScore, nCharNext(charSentence, st_index == -1 ? 0 : st_index, 2), scoreIndex, amount, trainingRound);
			m_map3CharAfterST.getOrUpdateScore(packedScore, nCharNext(charSentence, st_index == -1 ? 0 : st_index, 3), scoreIndex, amount, trainingRound);

			m_map1CharBeforeN0.getOrUpdateScore(packedScore, nCharPrev(charSentence, n0_index == -1 ? sentenceLength : n0_index, 1), scoreIndex, amount, trainingRound);
			m_map2CharBeforeN0.getOrUpdateScore(packedScore, nCharPrev(charSentence, n0_index == -1 ? sentenceLength : n0_index, 2), scoreIndex, amount, trainingRound);
			m_map3CharBeforeN0.getOrUpdateScore(packedScore, nCharPrev(charSentence, n0_index == -1 ? sentenceLength : n0_index, 3), scoreIndex, amount, trainingRound);
			m_map1CharAfterN0.getOrUpdateScore(packedScore, nCharNext(charSentence, n0_index == -1 ? sentenceLength : n0_index, 1), scoreIndex, amount, trainingRound);
			m_map2CharAfterN0.getOrUpdateScore(packedScore, nCharNext(charSentence, n0_index == -1 ? sentenceLength : n0_index, 2), scoreIndex, amount, trainingRound);
			m_map3CharAfterN0.getOrUpdateScore(packedScore, nCharNext(charSentence, n0_index == -1 ? sentenceLength : n0_index, 3), scoreIndex, amount, trainingRound);
		}

		if (parser->m_bPath) {

			LCA & analyzer = parser->m_lcaAnalyzer;
			const DependencyTree & syntaxTree = parser->m_dtSyntaxTree;
			const WordPOSTag & st_syn_head_word_postag = (st_index == -1 || TREENODE_HEAD(syntaxTree[st_index]) == -1 ? empty_taggedword : sentence[TREENODE_HEAD(syntaxTree[st_index])]);
			const WordPOSTag & n0_syn_head_word_postag = (n0_index == -1 || TREENODE_HEAD(syntaxTree[n0_index]) == -1 ? empty_taggedword : sentence[TREENODE_HEAD(syntaxTree[n0_index])]);
			const WordPOSTag & st2_syn_head_word_postag = (st2_index == -1 || TREENODE_HEAD(syntaxTree[st2_index]) == -1 ? empty_taggedword : sentence[TREENODE_HEAD(syntaxTree[st2_index])]);

			if (st_index == -1 || n0_index == -1) {
				m_mapSTPOSPath.getOrUpdateScore(packedScore, "n#", scoreIndex, amount, trainingRound);
				m_mapSTFPOSPath.getOrUpdateScore(packedScore, "n#", scoreIndex, amount, trainingRound);
			}
			else {
				m_mapSTPOSPath.getOrUpdateScore(packedScore, analyzer.POSPath[st_index][n0_index], scoreIndex, amount, trainingRound);
				m_mapSTFPOSPath.getOrUpdateScore(packedScore, analyzer.FPOSPath[st_index][n0_index], scoreIndex, amount, trainingRound);
			}
			tri_features.refer(st_word, n0_postag, st_syn_head_word_postag.second());
			m_mapSTwN0wSTsynhpt.getOrUpdateScore(packedScore, tri_features, scoreIndex, amount, trainingRound);
			tri_features.refer(st_word, n0_postag, n0_syn_head_word_postag.second());
			m_mapSTwN0wN0synhpt.getOrUpdateScore(packedScore, tri_features, scoreIndex, amount, trainingRound);

			if (st2_index == -1 || n0_index == -1) {
				m_mapST2POSPath.getOrUpdateScore(packedScore, "n#", scoreIndex, amount, trainingRound);
				m_mapST2FPOSPath.getOrUpdateScore(packedScore, "n#", scoreIndex, amount, trainingRound);
			}
			else {
				m_mapST2POSPath.getOrUpdateScore(packedScore, analyzer.POSPath[st2_index][n0_index], scoreIndex, amount, trainingRound);
				m_mapST2FPOSPath.getOrUpdateScore(packedScore, analyzer.FPOSPath[st2_index][n0_index], scoreIndex, amount, trainingRound);
			}
			tri_features.refer(st2_word, n0_postag, st2_syn_head_word_postag.second());
			m_mapST2wN0wST2synhpt.getOrUpdateScore(packedScore, tri_features, scoreIndex, amount, trainingRound);
			tri_features.refer(st2_word, n0_postag, n0_syn_head_word_postag.second());
			m_mapST2wN0wN0synhpt.getOrUpdateScore(packedScore, tri_features, scoreIndex, amount, trainingRound);

		}

		if (parser->m_bSuperTag) {
			const SuperTag & st_supertag = st_index == -1 ? emptySuperTag : item.superTag(st_index);
			const SuperTag & st2_supertag = st2_index == -1 ? emptySuperTag : item.superTag(st2_index);
			const SuperTag & n0l2_supertag = n0l2_index == -1 ? emptySuperTag : item.superTag(n0l2_index);
			const SuperTag & n0l1_supertag = n0l1_index == -1 ? emptySuperTag : item.superTag(n0l1_index);

			m_mapSTst.getOrUpdateScore(packedScore, st_supertag, scoreIndex, amount, trainingRound);
			m_mapST2st.getOrUpdateScore(packedScore, st2_supertag, scoreIndex, amount, trainingRound);
			bi_features.refer(n0l2_supertag, -2);
			m_mapN0ist.getOrUpdateScore(packedScore, bi_features, scoreIndex, amount, trainingRound);
			bi_features.refer(n0l1_supertag, -1);
			m_mapN0ist.getOrUpdateScore(packedScore, bi_features, scoreIndex, amount, trainingRound);
			bi_features.refer(st_supertag, st2_supertag);
			m_mapSTstST2st.getOrUpdateScore(packedScore, bi_features, scoreIndex, amount, trainingRound);
			bi_features.refer(st_supertag, n0_word);
			m_mapSTstN0w.getOrUpdateScore(packedScore, bi_features, scoreIndex, amount, trainingRound);
			bi_features.refer(st_supertag, n0_postag);
			m_mapSTstN0pt.getOrUpdateScore(packedScore, bi_features, scoreIndex, amount, trainingRound);
			tri_features.refer(st_supertag, n0_word, n0_postag);
			m_mapSTstN0wpt.getOrUpdateScore(packedScore, tri_features, scoreIndex, amount, trainingRound);
		}
	}
}

#endif
