#include "stdafx.h"
#include <cstring>

#include "arceager_state.h"

namespace arceager {

	StateItem::StateItem() {
		clear();
	}

	StateItem::~StateItem() = default;

	void StateItem::arcLeft(const int & l, const ArceagerConstant & ac) {
		int left = m_lStack[m_nStackBack--];
		--m_nHeadStackBack;
		m_lHeads[left] = m_nNextWord;
		m_lLabels[left] = l;
		m_lDepTagL[m_nNextWord].add(l);
		m_lSibling[left] = m_lDepsL[m_nNextWord];
		m_lDepsL[m_nNextWord] = left;
		++m_lDepsNumL[m_nNextWord];
		m_nLastAction = ac.AL_FIRST + l - 1;
	}

	void StateItem::arcRight(const int & l, const ArceagerConstant & ac) {
		int left = m_lStack[m_nStackBack];
		m_lStack[++m_nStackBack] = m_nNextWord;
		m_lHeads[m_nNextWord] = left;
		m_lLabels[m_nNextWord] = l;
		m_lDepTagR[left].add(l);
		m_lSibling[m_nNextWord] = m_lDepsR[left];
		m_lDepsR[left] = m_nNextWord++;
		++m_lDepsNumR[left];
		clearNext();
		m_nLastAction = ac.AR_FIRST + l - 1;
	}

	void StateItem::print(const ArceagerConstant & ac) const {
		std::cout << "complete" << std::endl;
		for (int i = 0; i < m_nNextWord; ++i) {
			std::cout << "index : " << i << " head : " << m_lHeads[i] << " label : " << m_lLabels[i] << std::endl;
		}
		if (m_nStackBack >= 0) std::cout << "stack :";
		for (int i = 0; i <= m_nStackBack; ++i) {
			std::cout << " " << m_lStack[i];
		}
		if (m_nStackBack >= 0) std::cout << std::endl;
		std::cout << "last action is ";
		printAction(m_nLastAction, ac);
		std::cout << "score is " << m_nScore << std::endl;
	}

	void StateItem::clear() {
		m_nNextWord = 0;
		m_nStackBack = -1;
		m_nHeadStackBack = -1;
		m_nScore = 0;
		m_nLastAction = NO_ACTION;
		clearNext();
	}

	void StateItem::clearNext() {
		m_lHeads[m_nNextWord] = -1;
		m_lDepsL[m_nNextWord] = -1;
		m_lDepsR[m_nNextWord] = -1;
		m_lDepsNumL[m_nNextWord] = 0;
		m_lDepsNumR[m_nNextWord] = 0;
		m_lDepTagL[m_nNextWord].clear();
		m_lDepTagR[m_nNextWord].clear();
		m_lSibling[m_nNextWord] = -1;
		m_lLabels[m_nNextWord] = 0;
	}

	void StateItem::move(const int & action, const ArceagerConstant & ac) {
		switch (decodeAction(action, ac)) {
		case NO_ACTION:
			return;
		case SHIFT:
			shift();
			return;
		case REDUCE:
			reduce();
			return;
		case ARC_LEFT:
			arcLeft(action - ac.AL_FIRST + 1, ac);
			return;
		case ARC_RIGHT:
			arcRight(action - ac.AR_FIRST + 1, ac);
			return;
		case POP_ROOT:
			popRoot();
			return;
		}
	}

	void StateItem::generateTree(const DependencyTree & sent, DependencyTree & tree, DLabel & labels) const {
		int i = 0;
		tree.clear();
		for (const auto & token : sent) {
			tree.push_back(DependencyTreeNode(TREENODE_POSTAGGEDWORD(token), m_lHeads[i], labels.key(m_lLabels[i])));
			++i;
		}
	}

	bool StateItem::standardMove(const DependencyTree & tree, DLabel & labels, const ArceagerConstant & ac) {
		int top;
		if (m_nNextWord == tree.size()) {
			if (m_nStackBack > 0) {
				reduce();
				return true;
			}
			else {
				popRoot();
				return false;
			}
		}
		if (m_nStackBack >= 0) {
			top = m_lStack[m_nStackBack];
			while (!(m_lHeads[top] == -1)) {
				top = m_lHeads[top];
			}
			if (TREENODE_HEAD(tree[top]) == m_nNextWord) {
				if (top == m_lStack[m_nStackBack]) {
					arcLeft(labels.lookup(TREENODE_LABEL(tree[top])), ac);
					return true;
				}
				else {
					reduce();
					return true;
				}
			}
		}
		if (TREENODE_HEAD(tree[m_nNextWord]) == -1 || TREENODE_HEAD(tree[m_nNextWord]) > m_nNextWord) {
			shift();
			return true;
		}
		else {
			top = m_lStack[m_nStackBack];
			if (TREENODE_HEAD(tree[m_nNextWord]) == top) {
				arcRight(labels.lookup(TREENODE_LABEL(tree[m_nNextWord])), ac);
				return true;
			}
			else {
				reduce();
				return true;
			}
		}
	}

	int StateItem::followMove(const StateItem & item, const ArceagerConstant & ac) {
		int top;
		if (m_nNextWord == item.m_nNextWord) {
			top = m_lStack[m_nStackBack];
			if (item.m_lHeads[top] == m_nNextWord) {
				return ac.AL_FIRST + item.m_lLabels[top] - 1;
			}
			else if (item.m_lHeads[top] != -1) {
				return REDUCE;
			}
			else {
				return POP_ROOT;
			}
		}
		if (m_nStackBack >= 0) {
			top = m_lStack[m_nStackBack];
			while (!(m_lHeads[top] == -1)) {
				top = m_lHeads[top];
			}
			if (item.m_lHeads[top] == m_nNextWord) {
				if (top == m_lStack[m_nStackBack]) {
					return ac.AL_FIRST + item.m_lLabels[top] - 1;
				}
				else {
					return REDUCE;
				}
			}
		}
		if (item.m_lHeads[m_nNextWord] == -1 || item.m_lHeads[m_nNextWord] > m_nNextWord) {
			return SHIFT;
		}
		else {
			top = m_lStack[m_nStackBack];
			if (item.m_lHeads[m_nNextWord] == top) {
				return ac.AR_FIRST + item.m_lLabels[m_nNextWord] - 1;
			}
			else {
				return REDUCE;
			}
		}
	}

	bool StateItem::operator==(const StateItem & item) const {
		if (m_nNextWord != item.m_nNextWord) {
			return false;
		}
		for (int i = 0; i < m_nNextWord; ++i) {
			if (m_lHeads[i] != item.m_lHeads[i] || m_lLabels[i] != item.m_lLabels[i]) {
				return false;
			}
		}
		if (m_nStackBack != item.m_nStackBack) {
			return false;
		}
		if (m_nStackBack >= 0 && m_lStack[m_nStackBack] != item.m_lStack[m_nStackBack]) {
			return false;
		}
		return true;
	}

	StateItem & StateItem::operator=(const StateItem & item) {
		m_nStackBack = item.m_nStackBack;
		m_nHeadStackBack = item.m_nHeadStackBack;
		m_nNextWord = item.m_nNextWord;
		m_nLastAction = item.m_nLastAction;
		m_nScore = item.m_nScore;

		size_t len = sizeof(int) * (m_nNextWord + 1);
		memcpy(m_lStack, item.m_lStack, sizeof(int) * (m_nStackBack + 1));
		memcpy(m_lHeadStack, item.m_lHeadStack, sizeof(int) * (m_nHeadStackBack + 1));

		memcpy(m_lHeads, item.m_lHeads, len);
		memcpy(m_lDepsL, item.m_lDepsL, len);
		memcpy(m_lDepsR, item.m_lDepsR, len);
		memcpy(m_lDepsNumL, item.m_lDepsNumL, len);
		memcpy(m_lDepsNumR, item.m_lDepsNumR, len);
		memcpy(m_lSibling, item.m_lSibling, len);
		memcpy(m_lLabels, item.m_lLabels, len);

		for (int index = 0; index <= m_nNextWord; ++index) {
			m_lDepTagL[index] = item.m_lDepTagL[index];
			m_lDepTagR[index] = item.m_lDepTagR[index];
		}

		return *this;
	}
}
