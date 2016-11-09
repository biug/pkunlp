#include "stdafx.h"

#include "graph_transition_state.h"

void GraphTransitionStateBase::arc(const int & label, const int & leftLabel, const int & rightLabel) {
	const int & left = m_lStack[m_nStackBack];
	// detect arc label
	// arc left
	if (leftLabel != 0) {
		arcLeft(leftLabel);
	}
	// arc right
	if (rightLabel != 0) {
		arcRight(rightLabel);
	}
	//add right arcs for stack seek
	m_vecRightArcs[left].push_back(DepRightArc(m_nNextWord, label));
}

void GraphTransitionStateBase::arcLeft(const int & label) {
	const int & left = m_lStack[m_nStackBack];
	//add new head for left and add label
	m_lHeadR[left] = m_nNextWord;
	m_lHeadLabelR[left] = label;
	++m_lHeadRNum[left];
	int & buffer_left_pred = m_lPredL[m_nNextWord];
	if (buffer_left_pred == -1) {
		buffer_left_pred = left;
		m_lPredLabelL[m_nNextWord] = label;
	}
	else if (left < buffer_left_pred) {
		m_lSubPredL[m_nNextWord] = buffer_left_pred;
		m_lSubPredLabelL[m_nNextWord] = m_lPredLabelL[m_nNextWord];
		buffer_left_pred = left;
		m_lPredLabelL[m_nNextWord] = label;
	}
	else {
		int & sub_buffer_left_pred = m_lSubPredL[m_nNextWord];
		if (sub_buffer_left_pred == -1 || left < sub_buffer_left_pred) {
			sub_buffer_left_pred = left;
			m_lSubPredLabelL[m_nNextWord] = label;
		}
	}
	++m_lPredLNum[m_nNextWord];
	m_lPredLabelSetL[m_nNextWord].add(label);
}

void GraphTransitionStateBase::arcRight(const int & label) {
	const int & left = m_lStack[m_nStackBack];
	//sibling is the previous child of buffer seek
	int & buffer_left_head = m_lHeadL[m_nNextWord];
	if (buffer_left_head == -1 || left < buffer_left_head) {
		buffer_left_head = left;
		m_lHeadLabelL[m_nNextWord] = label;
	}
	++m_lHeadLNum[m_nNextWord];
	m_lSubPredR[left] = m_lPredR[left];
	m_lPredR[left] = m_nNextWord;
	m_lSubPredLabelR[left] = m_lPredLabelR[left];
	m_lPredLabelR[left] = label;
	++m_lPredRNum[left];
	m_lPredLabelSetR[left].add(label);
}
