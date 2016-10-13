#ifndef _PACKED_SCORE_H
#define _PACKED_SCORE_H

#include <string>
#include <unordered_map>

#include "score.h"
#include "common/token/token.h"

template<typename KEY_TYPE, typename VAL_TYPE = Score, typename RET_TYPE = tscore, typename UPDATE_TYPE = int>
class PackedScoreMap {
private:
	std::unordered_map < KEY_TYPE, VAL_TYPE > m_mapScores;
	std::string m_sName;
	int m_nCount;

public:
	PackedScoreMap(const std::string & input_name = "") : m_sName(input_name), m_nCount(0) {}
	~PackedScoreMap() = default;

	bool hasKey(const KEY_TYPE & key) {
		return m_mapScores.find(key) != m_mapScores.end();
	}

	void updateScore(const KEY_TYPE & key, const UPDATE_TYPE & amount, const int & round) {
		m_mapScores[key].updateCurrent(amount, round);
	}

	void getOrUpdateScore(RET_TYPE & ts, const KEY_TYPE & key, const int & which, const UPDATE_TYPE & amount, const int & round) {
		if (amount == 0) {
			if (m_mapScores.find(key) != m_mapScores.end()) {
				m_mapScores[key].updateRetval(ts, which);
			}
		}
		else {
			updateScore(key, amount, round);
		}
	}

	void computeAverage(const int & round) {
		m_nCount = 0;
		for (auto && score : m_mapScores) {
			score.second.updateAverage(round);
			if (!score.second.zero()) {
				++m_nCount;
			}
		}
	}

	void clearScore() {
		for (auto && score : m_mapScores) {
			score.second.reset();
		}
	}

	void shrink(const double & round, const double & threshold) {
		for (auto && score : m_mapScores) {
			score.second.shrink(round, threshold);
		}
	}

	friend std::istream& operator>>(std::istream & is, PackedScoreMap<KEY_TYPE, VAL_TYPE, RET_TYPE, UPDATE_TYPE> & psm) {
		VAL_TYPE sc;
		int number;
		KEY_TYPE key;
		ttoken token;

		psm.m_mapScores.clear();
		is >> psm.m_sName >> psm.m_nCount;
		number = psm.m_nCount;
		while (number--) {
			is >> key >> token >> sc;
			psm.m_mapScores[key] = sc;
		}
		return is;
	}

	friend std::ostream& operator<<(std::ostream & os, const PackedScoreMap<KEY_TYPE, VAL_TYPE, RET_TYPE, UPDATE_TYPE> & psm) {
		os << psm.m_sName << " " << psm.m_nCount << std::endl;
		for (auto itr = psm.m_mapScores.begin(); itr != psm.m_mapScores.end(); ++itr) {
			if (!itr->second.zero()) {
				os << itr->first << " : " << itr->second << std::endl;
			}
		}
		return os;
	}
};

#endif
