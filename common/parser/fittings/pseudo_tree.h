#ifndef _PSEUDO_TREE_H
#define _PSEUDO_TREE_H

#include <stack>

#include "common/parser/macros_base.h"
#include "include/dependency_primitive.h"

#define CORRECT_ARC_SCORE	1000
#define ARC_SCORE(L,R,S)	((S) + (L) - (R))

#define PSEUDO_LABEL			("PSEUDO")
#define IS_FORE_LABEL(L)		((L).find("fore") == 0)
#define IS_BACK_LABEL(L)		((L).find("back") == 0)
#define ENCODE_FORE_LABEL(L)	("fore" + (L))
#define ENCODE_BACK_LABEL(L)	("back" + (L))
#define DECODE_FORE_LABEL(L)	((L).substr(strlen("fore")))
#define DECODE_BACK_LABEL(L)	((L).substr(strlen("back")))

class PseudoTreeFitting {
protected:

	typedef std::tuple<int, int, int> tEisnerArc;

	struct State {
		typedef std::pair<int, int> tScoreWithSplit;

		int left, right;
		tScoreWithSplit l2r, r2l, l2r_im, r2l_im;

		State(const int & l = -1, const int & r = -1) : left(l), right(r),
				l2r(0, -1), r2l(0, -1), l2r_im(0, -1), r2l_im(0, -1) {}
		State(const State & state) : left(state.left), right(state.right),
				l2r(state.l2r), r2l(state.r2l), l2r_im(state.l2r_im), r2l_im(state.r2l_im) {}
		~State() = default;

		void updateL2R(const int & score, const int & split) {
			if (l2r.second == -1 || l2r.first < score)
				l2r = tScoreWithSplit(score, split);
		}
		void updateR2L(const int & score, const int & split) {
			if (r2l.second == -1 || r2l.first < score)
				r2l = tScoreWithSplit(score, split);
		}
		void updateL2RIm(const int & score, const int & split) {
			if (l2r_im.second == -1 || l2r_im.first < score)
				l2r_im = tScoreWithSplit(score, split);
		}
		void updateR2LIm(const int & score, const int & split) {
			if (r2l_im.second == -1 || r2l_im.first < score)
				r2l_im = tScoreWithSplit(score, split);
		}
	};

	int arcScore(const int & head, const int & pred, const CoNLL08DepGraph & graph, const CoNLL08DepGraph & cGraph, const std::vector<CoNLL08DepGraph> & pGraphs, const std::vector<CoNLL08DepGraph> & nGraphs);
	ttoken arcLabel(const int & head, const int & pred, const CoNLL08DepGraph & graph);

	std::vector<tEisnerArc> decodeEisnerArcs(const CoNLL08DepGraph & graph, const CoNLL08DepGraph & cGraph, const std::vector<CoNLL08DepGraph> & pGraphs, const std::vector<CoNLL08DepGraph> & nGraphs);

public:

	DependencyTree extractPseudoTree(const CoNLL08DepGraph & graph, const std::vector<CoNLL08DepGraph> & pGraphs, const std::vector<CoNLL08DepGraph> & nGraphs, bool keepPseudo = false);
	DependencyTree extractPseudoTree(const CoNLL08DepGraph & graph, const CoNLL08DepGraph & cGraph, const std::vector<CoNLL08DepGraph> & pGraphs, const std::vector<CoNLL08DepGraph> & nGraphs, bool keepPseudo = false);
	DependencyPathTree extractPseudoScoreTree(const CoNLL08DepGraph & graph, const std::vector<CoNLL08DepGraph> & pGraphs, const std::vector<CoNLL08DepGraph> & nGraphs, bool keepPseudo = false);
	DependencyPathTree extractPseudoScoreTree(const CoNLL08DepGraph & graph, const CoNLL08DepGraph & cGraph, const std::vector<CoNLL08DepGraph> & pGraphs, const std::vector<CoNLL08DepGraph> & nGraphs, bool keepPseudo = false);
	CoNLL08DepGraph pseudoTreeToGraph(const DependencyTree & tree, bool keepPseudo = false);
	CoNLL08DepGraph addPseudoTag(const CoNLL08DepGraph & gold, const CoNLL08DepGraph & pseudo);
};

#endif
