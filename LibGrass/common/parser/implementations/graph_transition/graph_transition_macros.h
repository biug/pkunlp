#ifndef _GRAPH_TRANSITION_MACROS_BASE_H
#define _GRAPH_TRANSITION_MACROS_BASE_H

#include "stdafx.h"

#include <string>
#include <cstring>
#include <unordered_map>

#include "common/token/tagset.h"
#include "common/parser/macros_base.h"
#include "include/dependency_primitive.h"
#include "include/learning/perceptron/score_array.h"

// agenda
#define AGENDA_SIZE		16
// action start
#define ACTION_START	0

#define ENCODE_POSTAG_SET_2(T1, T2)		(((T1) << 8) | (T2))
#define ENCODE_POSTAG_SET_3(T1, T2, T3)	(((T1) << 16) | ((T2) << 8) | (T3))
inline unsigned int encodePOSTagSet4(const unsigned int & t1, const unsigned int & t2, const unsigned int & t3, const unsigned int & t4) {
	return ((t1 << 24) | (t2 << 16) | (t3 << 8) | t4);
}

typedef TagSetN<2> TagSet;

typedef CoNLL08DepNode DependencyGraphNode;
typedef CoNLL08DepGraph DependencyGraph;

typedef tscore PackedScoreType[600];
typedef tscore PackedSuperTagScoreType[30000];

typedef TriGram<unsigned int> IntTagSet;
typedef QuarGram<unsigned int> TwoIntsTagSet;

template<class RET_TYPE> using WordTagsetMap = PackedScoreMap<IntTagSet, ActionScores<RET_TYPE>, RET_TYPE, ActionScoreIncrement>;
template<class RET_TYPE> using POSTagTagsetMap = PackedScoreMap<IntTagSet, ActionScores<RET_TYPE>, RET_TYPE, ActionScoreIncrement>;

template<class RET_TYPE> using TwoWordsTagsetMap = PackedScoreMap<TwoIntsTagSet, ActionScores<RET_TYPE>, RET_TYPE, ActionScoreIncrement>;
template<class RET_TYPE> using POSTagSet2TagsetMap = PackedScoreMap<IntTagSet, ActionScores<RET_TYPE>, RET_TYPE, ActionScoreIncrement>;

template<class RET_TYPE> using IntMap = PackedScoreMap<Int, ActionScores<RET_TYPE>, RET_TYPE, ActionScoreIncrement>;
template<class RET_TYPE> using WordMap = PackedScoreMap<Int, ActionScores<RET_TYPE>, RET_TYPE, ActionScoreIncrement>;
template<class RET_TYPE> using POSTagMap = PackedScoreMap<Int, ActionScores<RET_TYPE>, RET_TYPE, ActionScoreIncrement>;
template<class RET_TYPE> using SuperTagMap = PackedScoreMap<Int, ActionScores<RET_TYPE>, RET_TYPE, ActionScoreIncrement>;
template<class RET_TYPE> using TwoWordsMap = PackedScoreMap<TwoInts, ActionScores<RET_TYPE>, RET_TYPE, ActionScoreIncrement>;
template<class RET_TYPE> using ThreeWordsMap = PackedScoreMap<ThreeInts, ActionScores<RET_TYPE>, RET_TYPE, ActionScoreIncrement>;
template<class RET_TYPE> using WordPOSTagMap = PackedScoreMap<TwoInts, ActionScores<RET_TYPE>, RET_TYPE, ActionScoreIncrement>;
template<class RET_TYPE> using WordWordPOSTagMap = PackedScoreMap<ThreeInts, ActionScores<RET_TYPE>, RET_TYPE, ActionScoreIncrement>;
template<class RET_TYPE> using WordPOSTagPOSTagMap = PackedScoreMap<TwoInts, ActionScores<RET_TYPE>, RET_TYPE, ActionScoreIncrement>;
template<class RET_TYPE> using WordPOSTagPOSTagIntMap = PackedScoreMap<ThreeInts, ActionScores<RET_TYPE>, RET_TYPE, ActionScoreIncrement>;
template<class RET_TYPE> using WordWordPOSTagPOSTagMap = PackedScoreMap<ThreeInts, ActionScores<RET_TYPE>, RET_TYPE, ActionScoreIncrement>;

template<class RET_TYPE> using POSTagSet2Map = PackedScoreMap<Unsigned, ActionScores<RET_TYPE>, RET_TYPE, ActionScoreIncrement>;
template<class RET_TYPE> using POSTagSet3Map = PackedScoreMap<Unsigned, ActionScores<RET_TYPE>, RET_TYPE, ActionScoreIncrement>;
template<class RET_TYPE> using POSTagSet4Map = PackedScoreMap<Unsigned, ActionScores<RET_TYPE>, RET_TYPE, ActionScoreIncrement>;

template<class RET_TYPE> using StringMap = PackedScoreMap<std::string, ActionScores<RET_TYPE>, RET_TYPE, ActionScoreIncrement>;
template<class RET_TYPE> using WordIntMap = PackedScoreMap<TwoInts, ActionScores<RET_TYPE>, RET_TYPE, ActionScoreIncrement>;
template<class RET_TYPE> using POSTagIntMap = PackedScoreMap<TwoInts, ActionScores<RET_TYPE>, RET_TYPE, ActionScoreIncrement>;
template<class RET_TYPE> using SuperTagIntMap = PackedScoreMap<TwoInts, ActionScores<RET_TYPE>, RET_TYPE, ActionScoreIncrement>;
template<class RET_TYPE> using SuperTagSet2Map = PackedScoreMap<TwoInts, ActionScores<RET_TYPE>, RET_TYPE, ActionScoreIncrement>;
template<class RET_TYPE> using TwoWordsIntMap = PackedScoreMap<ThreeInts, ActionScores<RET_TYPE>, RET_TYPE, ActionScoreIncrement>;
template<class RET_TYPE> using WordPOSTagIntMap = PackedScoreMap<ThreeInts, ActionScores<RET_TYPE>, RET_TYPE, ActionScoreIncrement>;
template<class RET_TYPE> using WordSuperTagMap = PackedScoreMap<TwoInts, ActionScores<RET_TYPE>, RET_TYPE, ActionScoreIncrement>;
template<class RET_TYPE> using POSTagSuperTagMap = PackedScoreMap<TwoInts, ActionScores<RET_TYPE>, RET_TYPE, ActionScoreIncrement>;
template<class RET_TYPE> using WordPOSTagSuperTagMap = PackedScoreMap<ThreeInts, ActionScores<RET_TYPE>, RET_TYPE, ActionScoreIncrement>;
template<class RET_TYPE> using ThreeWordsIntMap = PackedScoreMap<FourInts, ActionScores<RET_TYPE>, RET_TYPE, ActionScoreIncrement>;
template<class RET_TYPE> using POSTagSet2IntMap = PackedScoreMap<TwoInts, ActionScores<RET_TYPE>, RET_TYPE, ActionScoreIncrement>;
template<class RET_TYPE> using POSTagSet3IntMap = PackedScoreMap<TwoInts, ActionScores<RET_TYPE>, RET_TYPE, ActionScoreIncrement>;
template<class RET_TYPE> using POSTagSet4TwoIntsMap = PackedScoreMap<ThreeInts, ActionScores<RET_TYPE>, RET_TYPE, ActionScoreIncrement>;

std::istream & operator>>(std::istream & input, SuperTagCandidates & stc);
std::ostream & operator<<(std::ostream & output, const SuperTagCandidates & std);

template<class ACTION_TYPE>
void loadLabels(ACTION_TYPE & actions, const DLabel & labels) {
	int count = 0;
	actions.m_vecLabelMap.clear();
	actions.m_vecLabelMap.push_back(0);
	actions.LabelCount = labels.count();
	std::unordered_map<std::string, int> labelMap;

	for (int i = labels.start(); i < labels.end(); ++i) {
		const ttoken & label = labels.key(i);
		if (IS_LEFT_LABEL(label)) {
			if (labelMap.find(DECODE_LEFT_LABEL(label)) == labelMap.end()) {
				labelMap[DECODE_LEFT_LABEL(label)] = ++count;
			}
			actions.m_vecLabelMap.push_back(ENCODE_LABEL_ID(labelMap[DECODE_LEFT_LABEL(label)], 0));
		}
		else if (IS_RIGHT_LABEL(label)) {
			if (labelMap.find(DECODE_RIGHT_LABEL(label)) == labelMap.end()) {
				labelMap[DECODE_RIGHT_LABEL(label)] = ++count;
			}
			actions.m_vecLabelMap.push_back(ENCODE_LABEL_ID(0, labelMap[DECODE_RIGHT_LABEL(label)]));
		}
		else if (IS_TWOWAY_LABEL(label)) {
			if (labelMap.find(DECODE_TWOWAY_LEFT_LABEL(label)) == labelMap.end()) {
				labelMap[DECODE_TWOWAY_LEFT_LABEL(label)] = ++count;
			}
			if (labelMap.find(DECODE_TWOWAY_RIGHT_LABEL(label)) == labelMap.end()) {
				labelMap[DECODE_TWOWAY_RIGHT_LABEL(label)] = ++count;
			}
			actions.m_vecLabelMap.push_back(ENCODE_LABEL_ID(labelMap[DECODE_TWOWAY_LEFT_LABEL(label)], labelMap[DECODE_TWOWAY_RIGHT_LABEL(label)]));
		}
	}
}

#endif
