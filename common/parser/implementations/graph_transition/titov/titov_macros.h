#ifndef _TITOV_MACROS_H
#define _TITOV_MACROS_H

#include "stdafx.h"

#include "titov_state.h"
#include "common/parser/agenda.h"

namespace titov {

	enum Action {
		NO_ACTION = ACTION_START,
		A_SW,
		A_SH,
		A_RE,
		SWAP,
		REDUCE,
		SHIFT,
	};

	struct ActionConstant {

		int LabelCount;
		int m_nSuperTagCount;
		std::vector<int> m_vecLabelMap;
		int A_SW_FIRST, A_SW_END;
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
