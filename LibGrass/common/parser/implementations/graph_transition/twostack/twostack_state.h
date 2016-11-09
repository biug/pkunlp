#ifndef _TWOSTACK_STATE_H
#define _TWOSTACK_STATE_H

#include "common/parser/implementations/graph_transition/graph_transition_state.h"

namespace twostack {
	class StateItem : public GraphTransitionStateBase {

	private:
		bool m_bCanMem;
		int m_nSecondStackBack;
		int m_lSecondStack[MAX_SENTENCE_SIZE];

	public:
		StateItem();
		~StateItem();

		void reduce(const int & action);
		void mem(const int & action);
		void recall(const int & action);
		void shift(const int & tag, const int & action);
		void arc(const int & label, const int & leftLabel, const int & rightLabel, const int & action);
		void arcMem(const int & label, const int & leftLabel, const int & rightLabel, const int & action);
		void arcRecall(const int & label, const int & leftLabel, const int & rightLabel, const int & action);
		void arcReduce(const int & label, const int & leftLabel, const int & rightLabel, const int & action);
		void arcShift(const int & label, const int & leftLabel, const int & rightLabel, const int & tag, const int & action);

		const int & secondStackTop() const	{ return m_lSecondStack[m_nSecondStackBack]; }
		const int & secondStackBack() const {return m_nSecondStackBack; }

		bool canMem() const		{ return m_nStackBack > 0 && m_bCanMem; }
		bool canRecall() const	{ return m_nSecondStackBack >= 0; }
		bool canArc() const		{ return m_nStackBack == -1 ? false : (m_vecRightArcs[m_lStack[m_nStackBack]].empty() ? true : (m_vecRightArcs[m_lStack[m_nStackBack]].back().head != m_nNextWord)); }
		bool canShift() const	{ return m_nSecondStackBack == -1; }

		void clear();
		void clearNext();
		void print(const DLabel & labels, const DSuperTag & supertags) const override;

		bool operator<(const StateItem & item) const	{ return m_nScore < item.m_nScore; }
		bool operator<=(const StateItem & item) const	{ return m_nScore <= item.m_nScore; }
		bool operator>(const StateItem & item) const	{ return m_nScore > item.m_nScore; }
		bool operator!=(const StateItem & item) const	{ return !(*this == item); }

		bool operator==(const StateItem & item) const;
		bool operator==(const DependencyGraph & graph) const;

		StateItem & operator=(const StateItem & i);
	};
}

#endif
