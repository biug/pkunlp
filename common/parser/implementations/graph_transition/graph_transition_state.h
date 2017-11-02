#ifndef _GRAPH_TRANSITION_STATE_BASE_H
#define _GRAPH_TRANSITION_STATE_BASE_H

#include "stdafx.h"

#include "graph_transition_macros.h"

class GraphTransitionStateBase {
protected:
	tscore m_nScore;

	int m_nNextWord;
	int m_nStackBack;

	int m_nActionBack;
	int m_lActionList[MAX_SENTENCE_SIZE << MAX_SENTENCE_BITS];

	int m_lStack[MAX_SENTENCE_SIZE];
	int m_lHeadL[MAX_SENTENCE_SIZE];	//heads for every node
	int m_lHeadLabelL[MAX_SENTENCE_SIZE];	//label for every node
	int m_lHeadLNum[MAX_SENTENCE_SIZE];
	int m_lHeadR[MAX_SENTENCE_SIZE];
	int m_lHeadLabelR[MAX_SENTENCE_SIZE];
	int m_lHeadRNum[MAX_SENTENCE_SIZE];
	int m_lPredL[MAX_SENTENCE_SIZE];		//left dependency children
	int m_lSubPredL[MAX_SENTENCE_SIZE];
	int m_lPredLabelL[MAX_SENTENCE_SIZE];
	int m_lSubPredLabelL[MAX_SENTENCE_SIZE];
	int m_lPredLNum[MAX_SENTENCE_SIZE];
	int m_lPredR[MAX_SENTENCE_SIZE];		//right dependency children
	int m_lSubPredR[MAX_SENTENCE_SIZE];
	int m_lPredLabelR[MAX_SENTENCE_SIZE];
	int m_lSubPredLabelR[MAX_SENTENCE_SIZE];
	int m_lPredRNum[MAX_SENTENCE_SIZE];
	int m_lSuperTag[MAX_SENTENCE_SIZE];
	TagSet m_lPredLabelSetL[MAX_SENTENCE_SIZE];
	TagSet m_lPredLabelSetR[MAX_SENTENCE_SIZE];

	std::vector<DepRightArc> m_vecRightArcs[MAX_SENTENCE_SIZE];

public:
	GraphTransitionStateBase() : m_nScore(0), m_nNextWord(0), m_nStackBack(-1), m_nActionBack(-1) {}
	virtual ~GraphTransitionStateBase() {};

	const tscore & getScore() const;
	void setScore(const tscore & s);

	const int & action(const int & index) const;
	const int & stack(const int & index) const;
	const int & leftHead(const int & index) const;
	const int & rightHead(const int & index) const;
	const int & leftHeadLabel(const int & index) const;
	const int & rightHeadLabel(const int & index) const;
	const int & leftPred(const int & index) const;
	const int & rightPred(const int & index) const;
	const int & leftSubPred(const int & index) const;
	const int & rightSubPred(const int & index) const;
	const int & leftPredLabel(const int & index) const;
	const int & rightPredLabel(const int & index) const;
	const int & leftSubPredLabel(const int & index) const;
	const int & rightSubPredLabel(const int & index) const;
	const int & leftHeadArity(const int & index) const;
	const int & leftPredArity(const int & index) const;
	const int & rightHeadArity(const int & index) const;
	const int & rightPredArity(const int & index) const;
	const int & superTag(const int & index) const;
	const TagSet & leftPredLabelSet(const int & index) const;
	const TagSet & rightPredLabelSet(const int & index) const;

	const int & size() const;
	const int & stackBack() const;
	const int & stackTop() const;
	const int & stackSubTop() const;
	const int & actionBack() const;
	bool stackEmpty() const;

	void arc(const int & label, const int & leftLabel, const int & rightLabel);
	void arcLeft(const int & label);
	void arcRight(const int & label);
	void generateGraph(const DependencyGraph & sent, DependencyGraph & tree, const DLabel & labels, const DSuperTag & supertags) const;

	virtual void print(const DLabel & labels, const DSuperTag & supertags) const = 0;
};

inline const tscore & GraphTransitionStateBase::getScore() const {
	return m_nScore;
}

inline void GraphTransitionStateBase::setScore(const tscore & s) {
	m_nScore = s;
}

inline const int & GraphTransitionStateBase::action(const int & index) const {
	return m_lActionList[index];
}

inline const int & GraphTransitionStateBase::stack(const int & index) const {
	return m_lStack[index];
}

inline const int & GraphTransitionStateBase::leftHead(const int & index) const {
	return m_lHeadL[index];
}

inline const int & GraphTransitionStateBase::rightHead(const int & index) const {
	return m_lHeadR[index];
}

inline const int & GraphTransitionStateBase::leftHeadLabel(const int & index) const {
	return m_lHeadLabelL[index];
}

inline const int & GraphTransitionStateBase::rightHeadLabel(const int & index) const {
	return m_lHeadLabelR[index];
}

inline const int & GraphTransitionStateBase::leftPred(const int & index) const {
	return m_lPredL[index];
}

inline const int & GraphTransitionStateBase::rightPred(const int & index) const {
	return m_lPredR[index];
}

inline const int & GraphTransitionStateBase::leftSubPred(const int & index) const {
	return m_lSubPredL[index];
}

inline const int & GraphTransitionStateBase::rightSubPred(const int & index) const {
	return m_lSubPredR[index];
}

inline const int & GraphTransitionStateBase::leftPredLabel(const int & index) const {
	return m_lPredLabelL[index];
}

inline const int & GraphTransitionStateBase::rightPredLabel(const int & index) const {
	return m_lPredLabelR[index];
}

inline const int & GraphTransitionStateBase::leftSubPredLabel(const int & index) const {
	return m_lSubPredLabelL[index];
}

inline const int & GraphTransitionStateBase::rightSubPredLabel(const int & index) const {
	return m_lSubPredLabelR[index];
}

inline const int & GraphTransitionStateBase::leftHeadArity(const int & index) const {
	return m_lHeadLNum[index];
}

inline const int & GraphTransitionStateBase::leftPredArity(const int & index) const {
	return m_lPredLNum[index];
}

inline const int & GraphTransitionStateBase::rightHeadArity(const int & index) const {
	return m_lHeadRNum[index];
}

inline const int & GraphTransitionStateBase::rightPredArity(const int & index) const {
	return m_lPredRNum[index];
}

inline const int & GraphTransitionStateBase::superTag(const int & index) const {
	return m_lSuperTag[index];
}

inline const TagSet & GraphTransitionStateBase::leftPredLabelSet(const int & index) const {
	return m_lPredLabelSetL[index];
}

inline const TagSet & GraphTransitionStateBase::rightPredLabelSet(const int & index) const {
	return m_lPredLabelSetR[index];
}

inline const int & GraphTransitionStateBase::size() const {
	return m_nNextWord;
}

inline const int & GraphTransitionStateBase::stackBack() const {
	return m_nStackBack;
}

inline const int & GraphTransitionStateBase::stackSubTop() const {
	return m_lStack[m_nStackBack - 1];
}

inline const int & GraphTransitionStateBase::stackTop() const {
	return m_lStack[m_nStackBack];
}

inline const int & GraphTransitionStateBase::actionBack() const {
	return m_nActionBack;
}

inline bool GraphTransitionStateBase::stackEmpty() const {
	return m_nStackBack == -1;
}

inline void GraphTransitionStateBase::generateGraph(const DependencyGraph & sent, DependencyGraph & tree, const DLabel & labels, const DSuperTag & supertags) const {
	for (int i = 0, n = sent.size(); i < n; ++i) {
		tree.add(sent[i]);
		tree.back().m_vecRightArcs.clear();
		for (const auto & arc : m_vecRightArcs[i]) {
			tree.back().m_vecRightArcs.push_back(std::pair<int, ttokenold>(arc.head, labels.key(arc.label)));
		}
		if (supertags.count() > 0) {
			tree.back().m_sSuperTag = supertags.key(m_lSuperTag[i]);
		}
	}
}

#endif
