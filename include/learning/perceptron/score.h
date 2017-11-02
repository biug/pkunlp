#ifndef _SCORE_H
#define _SCORE_H

#include <string>
#include <iostream>
#include <algorithm>
#include <cmath>
#include <unordered_map>

#include "common/oldtoken/token.h"
#include "common/parser/macros_base.h"

typedef int cscore;
typedef long long tscore;

typedef std::pair<int, const int &> ActionScoreIncrement;

inline bool operator==(const ActionScoreIncrement & s, const int & i) {
	return s.second == i;
}

enum ScoreType {
	eNonAverage,
	eAverage,
};

class Score {

private:
	cscore m_nCurrent;
	tscore m_nTotal;

	int m_nLastUpdate;

public:
	Score(const cscore & c = 0, const tscore & t = 0);
	Score(const Score & s);
	~Score();

	void reset();
	bool empty() const;
	bool zero() const;
	tscore operator[](const int & which) const;
	void updateCurrent(const int & added = 0, const int & round = 0);
	void updateAverage(const int & round);
	void updateRetval(tscore & retval, const int & which);
	void updateRetvalCurrent(tscore & retval);
	void updateRetvalTotal(tscore & retval);
	void shrink(const double & round, const double & threshold);

	friend std::istream & operator>>(std::istream & is, Score & s) {
		ttokenold token;
		is >> s.m_nCurrent >> token >> s.m_nTotal;
		return is;
	}

	friend std::ostream & operator<<(std::ostream & os, const Score & s) {
		os << s.m_nCurrent << " / " << s.m_nTotal;
		return os;
	}
};

inline Score::Score(const cscore & c, const tscore & t) : m_nCurrent(c), m_nTotal(t), m_nLastUpdate(0) {}

inline Score::Score(const Score & s) : m_nCurrent(s.m_nCurrent), m_nTotal(s.m_nTotal), m_nLastUpdate(s.m_nLastUpdate) {}

inline Score::~Score() = default;

inline bool Score::empty() const {
	return m_nCurrent == 0 && m_nTotal == 0 && m_nLastUpdate == 0;
}

inline bool Score::zero() const {
	return m_nCurrent == 0 && m_nTotal == 0;
}

inline tscore Score::operator[](const int & which) const {
	switch (which) {
	case eNonAverage:
		return (tscore)m_nCurrent;
	case eAverage:
		return m_nTotal;
	default:
		return (tscore)m_nCurrent;
	}
}

inline void Score::reset() {
	m_nCurrent = 0;
	m_nTotal = (tscore)0;
	m_nLastUpdate = 0;
}

inline void Score::updateCurrent(const int & added, const int & round) {
	if (round > m_nLastUpdate) {
		updateAverage(round);
		m_nLastUpdate = round;
	}
	m_nCurrent += added;
	m_nTotal += added;
}

inline void Score::updateAverage(const int & round) {
	if (round > m_nLastUpdate) {
		m_nTotal += (tscore)(m_nCurrent * (round - m_nLastUpdate));
	}
}

inline void Score::updateRetval(tscore & retval, const int & which) {
	switch (which) {
	case eNonAverage:
		retval += (tscore)m_nCurrent;
		break;
	case eAverage:
		retval += m_nTotal;
		break;
	default:
		break;
	}
}

inline void Score::updateRetvalCurrent(tscore & retval) {
	retval += (tscore)m_nCurrent;
}

inline void Score::updateRetvalTotal(tscore & retval) {
	retval += m_nTotal;
}

inline void Score::shrink(const double & round, const double & threshold) {
	if (fabs((double)m_nTotal / (double)round) < threshold) {
		m_nCurrent = 0;
		m_nTotal = 0;
	}
}

class ScoreWithSplit {
private:
	int m_nSplit;
	tscore m_nScore;

public:
	ScoreWithSplit(const int & sp = -1, const tscore & sc = 0);
	ScoreWithSplit(const ScoreWithSplit & sws);
	~ScoreWithSplit();

	void reset();
	const int & getSplit() const;
	const tscore & getScore() const;
	void refer(const int & sp, const tscore & sc);
	ScoreWithSplit & operator=(const ScoreWithSplit & sws);

	bool operator<(const tscore & sc) const;
	bool operator<(const ScoreWithSplit & sws) const;
	bool operator<=(const ScoreWithSplit & sws) const;
	bool operator>(const ScoreWithSplit & sws) const;
};

inline ScoreWithSplit::ScoreWithSplit(const int & sp, const tscore & sc) : m_nSplit(sp), m_nScore(sc) {}

inline ScoreWithSplit::ScoreWithSplit(const ScoreWithSplit & sws) : ScoreWithSplit(sws.m_nSplit, sws.m_nScore) {}

inline ScoreWithSplit::~ScoreWithSplit() = default;

inline const int & ScoreWithSplit::getSplit() const {
	return m_nSplit;
}

inline const tscore & ScoreWithSplit::getScore() const {
	return m_nScore;
}

inline void ScoreWithSplit::reset() {
	m_nSplit = -1;
	m_nScore = 0;
}

inline void ScoreWithSplit::refer(const int & sp, const tscore & sc) {
	m_nSplit = sp;
	m_nScore = sc;
}

inline ScoreWithSplit & ScoreWithSplit::operator=(const ScoreWithSplit & sws) {
	m_nSplit = sws.m_nSplit;
	m_nScore = sws.m_nScore;
	return *this;
}

inline bool ScoreWithSplit::operator<(const tscore & s) const {
	return m_nSplit == -1 || m_nScore < s;
}

inline bool ScoreWithSplit::operator<(const ScoreWithSplit & sws) const {
	return m_nScore < sws.m_nScore;
}

inline bool ScoreWithSplit::operator<=(const ScoreWithSplit & sws) const {
	return m_nScore <= sws.m_nScore;
}

inline bool ScoreWithSplit::operator>(const ScoreWithSplit & sws) const {
	return m_nScore > sws.m_nScore;
}

class ScoreWithBiSplit {
private:
	int m_nSplit;
	int m_nInnerSplit;
	tscore m_nScore;

public:
	ScoreWithBiSplit(const int & sp = -1, const int & isp = -1, const tscore & sc = 0);
	ScoreWithBiSplit(const ScoreWithBiSplit & swbs);
	~ScoreWithBiSplit();

	void reset();
	const int & getSplit() const;
	const int & getInnerSplit() const;
	const tscore & getScore() const;
	void refer(const int & sp, const int & isp, const tscore & sc);
	ScoreWithBiSplit & operator=(const ScoreWithBiSplit & swbs);

	bool operator<(const tscore & sc) const;
	bool operator<(const ScoreWithBiSplit & sws) const;
	bool operator<=(const ScoreWithBiSplit & sws) const;
	bool operator>(const ScoreWithBiSplit & sws) const;
};

inline ScoreWithBiSplit::ScoreWithBiSplit(const int & sp, const int & isp, const tscore & sc) : m_nSplit(sp), m_nInnerSplit(isp), m_nScore(sc) {}

inline ScoreWithBiSplit::ScoreWithBiSplit(const ScoreWithBiSplit & swbs) : ScoreWithBiSplit(swbs.m_nSplit, swbs.m_nInnerSplit, swbs.m_nScore) {}

inline ScoreWithBiSplit::~ScoreWithBiSplit() = default;

inline const int & ScoreWithBiSplit::getSplit() const {
	return m_nSplit;
}

inline const int & ScoreWithBiSplit::getInnerSplit() const {
	return m_nInnerSplit;
}

inline const tscore & ScoreWithBiSplit::getScore() const {
	return m_nScore;
}

inline void ScoreWithBiSplit::reset() {
	m_nSplit = -1;
	m_nInnerSplit = -1;
	m_nScore = 0;
}

inline void ScoreWithBiSplit::refer(const int & sp, const int & isp, const tscore & sc) {
	m_nSplit = sp;
	m_nInnerSplit = isp;
	m_nScore = sc;
}

inline ScoreWithBiSplit & ScoreWithBiSplit::operator=(const ScoreWithBiSplit & swbs) {
	m_nSplit = swbs.m_nSplit;
	m_nInnerSplit = swbs.m_nInnerSplit;
	m_nScore = swbs.m_nScore;
	return *this;
}

inline bool ScoreWithBiSplit::operator<(const tscore & s) const {
	return m_nSplit == -1 || m_nScore < s;
}

inline bool ScoreWithBiSplit::operator<(const ScoreWithBiSplit & sws) const {
	if (m_nScore != sws.m_nScore) {
		return m_nScore < sws.m_nScore;
	}
	int l = IS_EMPTY(m_nSplit) ? 0 : 1;
	int r = IS_EMPTY(m_nSplit) ? 0 : 1;
	if (l != r || l == 0) {
		return l < r;
	}
	l = IS_EMPTY(m_nInnerSplit) ? 0 : 1;
	r = IS_EMPTY(m_nInnerSplit) ? 0 : 1;
	return l < r;
}

inline bool ScoreWithBiSplit::operator<=(const ScoreWithBiSplit & sws) const {
	if (m_nScore != sws.m_nScore) {
		return m_nScore < sws.m_nScore;
	}
	int l = IS_EMPTY(m_nSplit) ? 0 : 1;
	int r = IS_EMPTY(m_nSplit) ? 0 : 1;
	if (l != r || l == 0) {
		return l <= r;
	}
	l = IS_EMPTY(m_nInnerSplit) ? 0 : 1;
	r = IS_EMPTY(m_nInnerSplit) ? 0 : 1;
	return l <= r;
}

inline bool ScoreWithBiSplit::operator>(const ScoreWithBiSplit & sws) const {
	return !(*this <= sws);
}

class ScoreWithType {
private:
	int m_nType;
	tscore m_nScore;

public:
	ScoreWithType(const int & st = 1, const tscore & sc = 0);
	ScoreWithType(const ScoreWithType & swt);
	~ScoreWithType();

	const int & getType() const;
	const tscore & getScore() const;
	ScoreWithType & operator=(const ScoreWithType & swt);

	bool operator<(const ScoreWithType & swt) const;
	bool operator<=(const ScoreWithType & swt) const;
	bool operator>(const ScoreWithType & swt) const;
};

inline ScoreWithType::ScoreWithType(const int & st, const tscore & sc) : m_nType(st), m_nScore(sc) {}

inline ScoreWithType::ScoreWithType(const ScoreWithType & swt) : ScoreWithType(swt.m_nType, swt.m_nScore) {}

inline ScoreWithType::~ScoreWithType() = default;

inline const int & ScoreWithType::getType() const {
	return m_nType;
}

inline const tscore & ScoreWithType::getScore() const {
	return m_nScore;
}

inline ScoreWithType & ScoreWithType::operator=(const ScoreWithType & sws) {
	m_nType = sws.m_nType;
	m_nScore = sws.m_nScore;
	return *this;
}

inline bool ScoreWithType::operator<(const ScoreWithType & swt) const {
	return m_nScore < swt.m_nScore;
}

inline bool ScoreWithType::operator<=(const ScoreWithType & swt) const {
	return m_nScore <= swt.m_nScore;
}

inline bool ScoreWithType::operator>(const ScoreWithType & swt) const {
	return m_nScore > swt.m_nScore;
}

class ScoredAction {
private:
	int m_nAction;
	tscore m_nScore;

public:
	ScoredAction(const int & ac = 0, const tscore & sc = 0);
	ScoredAction(const ScoredAction & sa);
	~ScoredAction();

	const int & getAction() const;
	const tscore & getScore() const;
	ScoredAction & operator=(const ScoredAction & sa);

	bool operator<(const ScoredAction & swt) const;
	bool operator<=(const ScoredAction & swt) const;
	bool operator>(const ScoredAction & swt) const;
};

inline ScoredAction::ScoredAction(const int & ac, const tscore & sc) : m_nAction(ac), m_nScore(sc) {}

inline ScoredAction::ScoredAction(const ScoredAction & sa) : ScoredAction(sa.m_nAction, sa.m_nScore) {}

inline ScoredAction::~ScoredAction() = default;

inline const int & ScoredAction::getAction() const {
	return m_nAction;
}

inline const tscore & ScoredAction::getScore() const {
	return m_nScore;
}

inline ScoredAction & ScoredAction::operator=(const ScoredAction & sa) {
	m_nAction = sa.m_nAction;
	m_nScore = sa.m_nScore;
	return *this;
}

inline bool ScoredAction::operator<(const ScoredAction & sa) const {
	return m_nScore < sa.m_nScore;
}

inline bool ScoredAction::operator<=(const ScoredAction & sa) const {
	return m_nScore <= sa.m_nScore;
}

inline bool ScoredAction::operator>(const ScoredAction & sa) const {
	return m_nScore > sa.m_nScore;
}

#endif
