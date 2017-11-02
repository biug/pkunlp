#ifndef _SCORE_ARRAY_H
#define _SCORE_ARRAY_H

#include <unordered_map>

#include "score.h"
#include "packed_score.h"

// ActionScores
template<class RET_TYPE>
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
	void updateRetval(RET_TYPE & retval, const int & which);
	void shrink(const double & round, const double & threshold);
	std::unordered_map<int, Score> & getScores();
	const std::unordered_map<int, Score> & getScores() const;

	ActionScores<RET_TYPE> & operator=(const ActionScores<RET_TYPE> & s) {
		m_mapScores.clear();
		m_mapScores.insert(s.m_mapScores.begin(), s.m_mapScores.end());
		return *this;
	}
};

template<class RET_TYPE>
inline ActionScores<RET_TYPE>::ActionScores() = default;

template<class RET_TYPE>
inline ActionScores<RET_TYPE>::ActionScores(const ActionScores<RET_TYPE> & s) {
	m_mapScores.clear();
	m_mapScores.insert(s.m_mapScores.begin(), s.m_mapScores.end());
}

template<class RET_TYPE>
inline ActionScores<RET_TYPE>::~ActionScores() = default;

template<class RET_TYPE>
inline void ActionScores<RET_TYPE>::reset() {
	m_mapScores.clear();
}

template<class RET_TYPE>
bool ActionScores<RET_TYPE>::zero() const {
	for (const auto & score : m_mapScores) {
		if (!score.second.zero()) {
			return false;
		}
	}
	return true;
}

template<class RET_TYPE>
inline void ActionScores<RET_TYPE>::updateCurrent(const ActionScoreIncrement & added, const int & round) {
	m_mapScores[added.first].updateCurrent(added.second, round);
}

template<class RET_TYPE>
inline void ActionScores<RET_TYPE>::updateAverage(const int & round) {
	for (auto && score : m_mapScores) {
		score.second.updateAverage(round);
	}
}

template<class RET_TYPE>
void ActionScores<RET_TYPE>::updateRetval(RET_TYPE & retval, const int & which) {
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

template<class RET_TYPE>
void ActionScores<RET_TYPE>::shrink(const double & round, const double & threshold) {
	for (auto && score : m_mapScores) {
		score.second.shrink(round, threshold);
	}
}

template<class RET_TYPE>
inline std::unordered_map<int, Score> & ActionScores<RET_TYPE>::getScores() {
	return m_mapScores;
}

template<class RET_TYPE>
inline const std::unordered_map<int, Score> & ActionScores<RET_TYPE>::getScores() const {
	return m_mapScores;
}

template<class RET_TYPE>
std::istream & operator>>(std::istream & is, ActionScores<RET_TYPE> & s) {
	ttokenold token;
	int index;
	cscore cs;
	tscore ts;
	s.reset();
	is >> token;
	while (is >> index) {
		is >> token >> cs >> token >> ts >> token;
		s.getScores()[index] = Score(cs, ts);
		if (token == "}") {
			break;
		}
	}
	return is;
}

template<class RET_TYPE>
std::ostream & operator<<(std::ostream & os, const ActionScores<RET_TYPE> & s) {
	bool start = false;
	for (const auto & score : s.getScores()) {
		if (!score.second.zero()) {
			if (!start) {
				os << "{ ";
				start = true;
			}
			else {
				os << " , ";
			}
			os << score.first << " : " << score.second;
		}
	}
	os << " }";
	return os;
}

#endif
