#ifndef _TWOSTACK_MACROS_H
#define _TWOSTACK_MACROS_H

#include "twostack_state.h"
#include "common/parser/agenda.h"

namespace twostack {

	enum Action {
		NO_ACTION = ACTION_START,
		A_MM,
		A_RC,
		A_SH,
		A_RE,
		MEM,
		RECALL,
		REDUCE,
		SHIFT,
	};

	struct ActionConstant {

		int LabelCount;
		int m_nSuperTagCount;
		std::vector<int> m_vecLabelMap;
		int A_MM_FIRST, A_MM_END;
		int A_RC_FIRST, A_RC_END;
		int A_RE_FIRST, A_RE_END;
		int A_SH_FIRST, A_SH_END;
		int SH_FIRST, SH_END;

		ActionConstant(const int & superTagCount = 0);
		ActionConstant(const ActionConstant & actions);
		~ActionConstant();

		void loadConstant(const DLabel & labels);
		void doAction(StateItem * item, const int & action) const;
		bool extractOracle(StateItem * item, const DependencyGraph & graph) const;
		bool followOneAction(StateItem * item, int (&seeks)[MAX_SENTENCE_SIZE], const DependencyGraph & graph,
									const std::pair<int, std::pair<int, int>> & labels = {0, {0, 0}}) const;
		void print(const int & action) const;
		void print() const;
	};

}

#endif
