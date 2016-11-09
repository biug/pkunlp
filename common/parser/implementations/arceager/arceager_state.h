#ifndef _ARCEAGER_STATE_H
#define _ARCEAGER_STATE_H

#include "stdafx.h"

#include "arceager_macros.h"

namespace arceager {

	class StateItem {
	private:
		int m_nStackBack;
		int m_nHeadStackBack;
		int m_nNextWord;
		int m_nLastAction;
		tscore m_nScore;

		int m_lLabels[MAX_SENTENCE_SIZE];
		int m_lStack[MAX_SENTENCE_SIZE];
		int m_lHeadStack[MAX_SENTENCE_SIZE];
		int m_lHeads[MAX_SENTENCE_SIZE];
		int m_lDepsL[MAX_SENTENCE_SIZE];
		int m_lDepsR[MAX_SENTENCE_SIZE];
		int m_lDepsNumL[MAX_SENTENCE_SIZE];
		int m_lDepsNumR[MAX_SENTENCE_SIZE];
		int m_lSibling[MAX_SENTENCE_SIZE];

		SetOfDepLabels m_lDepTagL[MAX_SENTENCE_SIZE];
		SetOfDepLabels m_lDepTagR[MAX_SENTENCE_SIZE];

	public:
		StateItem();
		~StateItem();

		void arcLeft(const int & l, const ArceagerConstant & ac);
		void arcRight(const int & l, const ArceagerConstant & ac);
		void shift();
		void reduce();
		void popRoot();

		const tscore & getScore() const;
		void setScore(const tscore & s);

		const int & stackTop() const;
		const int & head(const int & index) const;
		const int & leftDep(const int & index) const;
		const int & rightDep(const int & index) const;
		const int & sibling(const int & index) const;
		const int & label(const int & index) const;
		const int & leftArity(const int & index) const;
		const int & rightArity(const int & index) const;
		const SetOfDepLabels & leftSetOfLabels(const int & index) const;
		const SetOfDepLabels & rightSetOfLabels(const int & index) const;

		const int & size() const;
		const int & stackBack() const;
		const int & headStackBack() const;
		const int & lastAction() const;

		bool afterReduce() const;
		bool stackEmpty() const;

		void print(const ArceagerConstant & ac) const;

		void clear();
		void clearNext();
		void move(const int & action, const ArceagerConstant & ac);
		void generateTree(const DependencyTree & sent, DependencyTree & tree, DLabel & labels) const;
		bool standardMove(const DependencyTree & tree, DLabel & labels, const ArceagerConstant & ac);
		int followMove(const StateItem & item, const ArceagerConstant & ac);

		bool operator<(const StateItem & i) const;
		bool operator<=(const StateItem & i) const;
		bool operator>(const StateItem & i) const;

		bool operator==(const StateItem & i) const;
		bool operator!=(const StateItem & i) const;
		StateItem & operator=(const StateItem & i);
	};

	inline void StateItem::shift() {
		m_lStack[++m_nStackBack] = m_nNextWord;
		m_lHeadStack[++m_nHeadStackBack] = m_nNextWord++;
		clearNext();
		m_nLastAction = SHIFT;
	}

	inline void StateItem::reduce() {
		--m_nStackBack;
		m_nLastAction = REDUCE;
	}

	inline void StateItem::popRoot() {
		m_lLabels[m_lStack[m_nStackBack--]] = 1;
		m_nLastAction = POP_ROOT;
	}

	inline const tscore & StateItem::getScore() const {
		return m_nScore;
	}

	inline void StateItem::setScore(const tscore & s) {
		m_nScore = s;
	}

	inline const int & StateItem::stackTop() const {
		return m_lStack[m_nStackBack];
	}

	inline const int & StateItem::head(const int & index) const {
		return m_lHeads[index];
	}

	inline const int & StateItem::leftDep(const int & index) const {
		return m_lDepsL[index];
	}

	inline const int & StateItem::rightDep(const int & index) const {
		return m_lDepsR[index];
	}

	inline const int & StateItem::sibling(const int & index) const {
		return m_lSibling[index];
	}

	inline const int & StateItem::label(const int & index) const {
		return m_lLabels[index];
	}

	inline const int & StateItem::leftArity(const int & index) const {
		return m_lDepsNumL[index];
	}

	inline const int & StateItem::rightArity(const int & index) const {
		return m_lDepsNumR[index];
	}

	inline const SetOfDepLabels & StateItem::leftSetOfLabels(const int & index) const {
		return m_lDepTagL[index];
	}

	inline const SetOfDepLabels & StateItem::rightSetOfLabels(const int & index) const {
		return m_lDepTagR[index];
	}

	inline const int & StateItem::size() const {
		return m_nNextWord;
	}

	inline const int & StateItem::stackBack() const {
		return m_nStackBack;
	}

	inline const int & StateItem::headStackBack() const {
		return m_nHeadStackBack;
	}

	inline const int & StateItem::lastAction() const {
		return m_nLastAction;
	}

	inline bool StateItem::afterReduce() const {
		return m_nLastAction == REDUCE;
	}

	inline bool StateItem::stackEmpty() const {
		return m_nStackBack == -1;
	}

	inline bool StateItem::operator<(const StateItem & item) const {
		return m_nScore < item.m_nScore;
	}

	inline bool StateItem::operator<=(const StateItem & item) const {
		return m_nScore <= item.m_nScore;
	}

	inline bool StateItem::operator>(const StateItem & item) const {
		return m_nScore > item.m_nScore;
	}

	inline bool StateItem::operator!=(const StateItem & item) const {
		return !(*this == item);
	}
}

#endif
