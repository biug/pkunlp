#ifndef _ARCEAGER_MACROS_H
#define _ARCEAGER_MACROS_H

#include "stdafx.h"

#include "common/token/tagset.h"
#include "common/parser/agenda.h"
#include "common/parser/macros_base.h"
#include "include/learning/perceptron/packed_score.h"

namespace arceager {

#define MAX_ACTION 200
	typedef tscore PackedScoreType[MAX_ACTION];
	typedef std::pair<int, const int &> ActionScoreIncrement;

	class ActionScores {

	private:
		std::unordered_map<int, Score> m_mapScores;

	public:
		ActionScores();
		ActionScores(const ActionScores & s);
		~ActionScores();

		void reset();
		bool zero() const;
		void updateCurrent(const ActionScoreIncrement & added, const int & round);
		void updateAverage(const int & round);
		void updateRetval(PackedScoreType & retval, const int & which);
		std::unordered_map<int, Score> & getScores();
		const std::unordered_map<int, Score> & getScores() const;

		ActionScores & operator=(const ActionScores & s) {
			m_mapScores.clear();
			m_mapScores.insert(s.m_mapScores.begin(), s.m_mapScores.end());
			return *this;
		}
	};

	inline ActionScores::ActionScores() = default;

	inline ActionScores::ActionScores(const ActionScores & s) {
		m_mapScores.clear();
		m_mapScores.insert(s.m_mapScores.begin(), s.m_mapScores.end());
	}

	inline ActionScores::~ActionScores() = default;

	inline void ActionScores::reset() {
		m_mapScores.clear();
	}

	inline void ActionScores::updateCurrent(const ActionScoreIncrement & added, const int & round) {
		m_mapScores[added.first].updateCurrent(added.second, round);
	}

	inline void ActionScores::updateAverage(const int & round) {
		for (auto && score : m_mapScores) {
			score.second.updateAverage(round);
		}
	}

	inline void ActionScores::updateRetval(PackedScoreType & retval, const int & which) {
		switch (which) {
		case eNonAverage:
			for (auto && score : m_mapScores) {
				score.second.updateRetvalCurrent(retval[score.first]);
			}
			break;
		case eAverage:
			for (auto && score : m_mapScores) {
				score.second.updateRetvalTotal(retval[score.first]);
			}
			break;
		}
	}

	inline std::unordered_map<int, Score> & ActionScores::getScores() {
		return m_mapScores;
	}

	inline const std::unordered_map<int, Score> & ActionScores::getScores() const {
		return m_mapScores;
	}
}

std::istream & operator>>(std::istream & is, arceager::ActionScores & s);
std::ostream & operator<<(std::ostream & os, const arceager::ActionScores & s);

namespace arceager {

	enum Action {
		NO_ACTION = 0,
		ARC_LEFT,
		ARC_RIGHT,
		SHIFT,
		REDUCE,
		POP_ROOT,
	};

	struct ArceagerConstant {
		int AL_FIRST, AL_END;
		int AR_FIRST, AR_END;
	};

#define ENCODE_POSTAG_SET_2(T1, T2)		(((T1) << 8) | (T2))
#define ENCODE_POSTAG_SET_3(T1, T2, T3)	(((T1) << 16) | ((T2) << 8) | (T3))

	typedef TagSetN<3> SetOfDepLabels;

	typedef PackedScoreMap<Int, ActionScores, PackedScoreType, ActionScoreIncrement> IntMap;
	typedef PackedScoreMap<Word, ActionScores, PackedScoreType, ActionScoreIncrement> WordMap;
	typedef PackedScoreMap<POSTag, ActionScores, PackedScoreType, ActionScoreIncrement> POSTagMap;
	typedef PackedScoreMap<TwoWords, ActionScores, PackedScoreType, ActionScoreIncrement> TwoWordsMap;
	typedef PackedScoreMap<WordPOSTag, ActionScores, PackedScoreType, ActionScoreIncrement> POSTaggedWordMap;
	typedef PackedScoreMap<WordWordPOSTag, ActionScores, PackedScoreType, ActionScoreIncrement> WordWordPOSTagMap;
	typedef PackedScoreMap<WordPOSTagPOSTag, ActionScores, PackedScoreType, ActionScoreIncrement> WordPOSTagPOSTagMap;
	typedef PackedScoreMap<WordWordPOSTagPOSTag, ActionScores, PackedScoreType, ActionScoreIncrement> TwoPOSTaggedWordsMap;

	typedef PackedScoreMap<Int, ActionScores, PackedScoreType, ActionScoreIncrement> POSTagSet2Map;
	typedef PackedScoreMap<Int, ActionScores, PackedScoreType, ActionScoreIncrement> POSTagSet3Map;

	typedef PackedScoreMap<QuarGram<Unsigned>, ActionScores, PackedScoreType, ActionScoreIncrement> WordSetOfDepLabelsMap;
	typedef PackedScoreMap<QuarGram<Unsigned>, ActionScores, PackedScoreType, ActionScoreIncrement> POSTagSetOfDepLabelsMap;

	typedef PackedScoreMap<WordInt, ActionScores, PackedScoreType, ActionScoreIncrement> WordIntMap;
	typedef PackedScoreMap<POSTagInt, ActionScores, PackedScoreType, ActionScoreIncrement> POSTagIntMap;

	typedef PackedScoreMap<TwoWordsInt, ActionScores, PackedScoreType, ActionScoreIncrement> WordWordIntMap;
	typedef PackedScoreMap<POSTagSet2Int, ActionScores, PackedScoreType, ActionScoreIncrement> POSTagPOSTagIntMap;

	class StateItem;

	typedef AgendaBeam<StateItem, 64> StateAgenda;
	typedef AgendaBeam<ScoredAction, 64> ScoreAgenda;

	int decodeAction(const int & action, const ArceagerConstant & ac);
	void printAction(const int & action, const ArceagerConstant & ac);
}

#endif
