#include <stack>
#include <sstream>
#include <unordered_set>

#include "twostack_macros.h"

namespace twostack {
	ActionConstant::ActionConstant(const int & superTagCount) :
			LabelCount(0), m_nSuperTagCount(superTagCount),
			A_MM_FIRST(SHIFT + 1), A_MM_END(A_MM_FIRST + LabelCount),
			A_RC_FIRST(A_MM_END), A_RC_END(A_RC_FIRST + LabelCount),
			A_RE_FIRST(A_RC_END), A_RE_END(A_RE_FIRST + LabelCount),
			A_SH_FIRST(A_RE_END), A_SH_END(A_SH_FIRST + (superTagCount + 1) * LabelCount),
			SH_FIRST(A_SH_END), SH_END(SH_FIRST + superTagCount + 1) {}

	ActionConstant::ActionConstant(const ActionConstant & actions) : ActionConstant(actions.m_nSuperTagCount) {}

	ActionConstant::~ActionConstant() = default;

	void ActionConstant::loadConstant(const DLabel & labels) {
		loadLabels(*this, labels);

		A_MM_FIRST = SHIFT + 1;	A_MM_END = A_MM_FIRST + LabelCount;
		A_RC_FIRST = A_MM_END;	A_RC_END = A_RC_FIRST + LabelCount;
		A_RE_FIRST = A_RC_END;	A_RE_END = A_RE_FIRST + LabelCount;
		A_SH_FIRST = A_RE_END;	A_SH_END = A_SH_FIRST + (m_nSuperTagCount + 1) * LabelCount;
		SH_FIRST = A_SH_END;	SH_END = SH_FIRST + m_nSuperTagCount + 1;
	}

	bool ActionConstant::extractOracle(StateItem * item, const DependencyGraph & graph) const {
		int rightNodeSeeks[MAX_SENTENCE_SIZE];
		memset(rightNodeSeeks, 0, sizeof(rightNodeSeeks));
		while (followOneAction(item, rightNodeSeeks, graph))
			;
		return *item == graph;
	}

	/*
	 	action priority is : "reduce" > "arc *" > "swap" > "shift"
		for example
		| 0 | 1 |					| 2 | 3 | . . .
		if we have arc 0 - 2, 1 - 3
		we use swap, get state
		| 1 |						| 2 | 3 | . . .
		| 0 |
		we use shift, get state
		| 1 | 0 |					| 2 | 3 | . . .
		we use arc-reduce, get state
		| 1 |						| 2 | 3 | . . .		"0 - 2"
		we use shift + reduce, get state
		| 1 |							| 3 | . . .
		finally we use arc-reduce, get state
		|								| 3 | . . .		"1 - 3"
	*/
	bool ActionConstant::followOneAction(StateItem * item, int (&seeks)[MAX_SENTENCE_SIZE], const DependencyGraph & graph,
								const std::pair<int, std::pair<int, int>> & labels) const {
		if (!item->stackEmpty()) {
			int & seek = seeks[item->stackTop()];
			const DependencyGraphNode & node = graph[item->stackTop()];
			int size = node.m_vecRightArcs.size();
			while (seek < size && node.m_vecRightArcs[seek].first < item->size()) {
				++seek;
			}
			if (seek >= size) {
				switch (labels.first) {
				case 0:
					item->reduce(REDUCE);
					return true;
				default:
					item->arcReduce(labels.first, labels.second.first, labels.second.second, A_RE_FIRST + labels.first - 1);
					return true;
				}
			}
			const auto & rightArc = node.m_vecRightArcs[seek];
			if (rightArc.first == item->size()) {
				++seek;
				return followOneAction(item, seeks, graph, node.m_vecRightLabels[seek - 1]);
			}
		}
		// swap after reduce/arc
		for (int i = item->stackBack() - 1; i >= 0; --i) {
			const DependencyGraphNode & node = graph[item->stack(i)];
			const int & seek = seeks[item->stack(i)];
			if (seek < node.m_vecRightArcs.size() && node.m_vecRightArcs[seek].first == item->size()) {
				switch (labels.first) {
				case 0:
					item->mem(MEM);
					return true;
				default:
					item->arcMem(labels.first, labels.second.first, labels.second.second, A_MM_FIRST + labels.first - 1);
					return true;
				}
			}
		}
		for (int i = item->secondStackBack(); i >= 0; --i) {
			switch (labels.first) {
			case 0:
				item->recall(RECALL);
				return true;
			default:
				item->arcRecall(labels.first, labels.second.first, labels.second.second, A_RC_FIRST + labels.first - 1);
				return true;
			}
		}
		// shfit after swap
		if (item->size() < graph.size()) {
			const int & tag = graph[item->size()].m_nSuperTagCode;
			switch (labels.first) {
			case 0:
				item->shift(tag, SH_FIRST + tag);
				return true;
			default:
				item->arcShift(labels.first, labels.second.first, labels.second.second, tag,
						A_SH_FIRST + tag * LabelCount + labels.first - 1);
				return true;
			}
		}
		return false;
	}

	void ActionConstant::doAction(StateItem * item, const int & action) const {
		if (action <= SHIFT) {
			switch (action) {
			case MEM:
				item->mem(action);
				return;
			case RECALL:
				item->recall(action);
				return;
			case REDUCE:
				item->reduce(action);
				return;
			default:
				return;
			}
		}
		else if (action < A_MM_END) {
			int label = action - A_MM_FIRST + 1;
			int labelId = m_vecLabelMap[label];
			item->arcMem(label, LEFT_LABEL_ID(labelId), RIGHT_LABEL_ID(labelId), action);
		}
		else if (action < A_RC_END) {
			int label = action - A_RC_FIRST + 1;
			int labelId = m_vecLabelMap[label];
			item->arcRecall(label, LEFT_LABEL_ID(labelId), RIGHT_LABEL_ID(labelId), action);
		}
		else if (action < A_RE_END) {
			int label = action - A_RE_FIRST + 1;
			int labelId = m_vecLabelMap[label];
			item->arcReduce(label, LEFT_LABEL_ID(labelId), RIGHT_LABEL_ID(labelId), action);
		}
		else if (action < A_SH_END) {
			if (m_nSuperTagCount > 0) {
				int label = (action - A_SH_FIRST) % LabelCount + 1;
				int labelId = m_vecLabelMap[label];
				item->arcShift(label, LEFT_LABEL_ID(labelId), RIGHT_LABEL_ID(labelId), (action - A_SH_FIRST) / LabelCount, action);
			}
			else {
				int label = action - A_SH_FIRST + 1;
				int labelId = m_vecLabelMap[label];
				item->arcShift(label, LEFT_LABEL_ID(labelId), RIGHT_LABEL_ID(labelId), 0, action);
			}
		}
		else {
			item->shift(action - SH_FIRST, action);
		}
	}

	void ActionConstant::print(const int & action) const {
		if (action <= SHIFT) {
			switch (action) {
			case NO_ACTION:
				std::cout << "no action";
				break;
			case MEM:
				std::cout << "mem";
				break;
			case RECALL:
				std::cout << "recall";
				break;
			case REDUCE:
				std::cout << "reduce";
				break;
			default:
				std::cout << "wrong action";
				break;
			}
		}
		else if (action < A_MM_END) {
			std::cout << "arc mem with label " << action - A_MM_FIRST + 1;
		}
		else if (action < A_RC_END) {
			std::cout << "arc recall with label " << action - A_RC_FIRST + 1;
		}
		else if (action < A_RE_END) {
			std::cout << "arc reduce with label " << action - A_RE_FIRST + 1;
		}
		else if (action < A_SH_END) {
			std::cout << "arc shift with label " << (action - A_SH_FIRST) % LabelCount + 1 << " with tag " << (action - A_SH_FIRST) / LabelCount;
		}
		else if (action < SH_END) {
			std::cout << "shift with tag " << action - SH_FIRST;
		}
		else {
			std::cout << "wrong action";
		}
		std::cout << " (" << action << ")" << std::endl;
	}

	void ActionConstant::print() const {
		std::cout << "label count is " << LabelCount << std::endl;
		std::cout << "arc mem first is " << A_MM_FIRST << std::endl;
		std::cout << "arc mem end is " << A_MM_END << std::endl;
		std::cout << "arc recall first is " << A_RC_FIRST << std::endl;
		std::cout << "arc recall end is " << A_RC_END << std::endl;
		std::cout << "arc reduce first is " << A_RE_FIRST << std::endl;
		std::cout << "arc reduce end is " << A_RE_END << std::endl;
		std::cout << "arc shift first is " << A_SH_FIRST << std::endl;
		std::cout << "arc shift end is " << A_SH_END << std::endl;
		std::cout << "shift first is " << SH_FIRST << std::endl;
		std::cout << "shift end is " << SH_END << std::endl;
	}
}
