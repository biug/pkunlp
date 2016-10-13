#include "pseudo_tree.h"

int PseudoTreeFitting::arcScore(const int & head, const int & pred, const CoNLL08DepGraph & goldGraph, const CoNLL08DepGraph & cGraph, const std::vector<CoNLL08DepGraph> & pGraphs, const std::vector<CoNLL08DepGraph> & nGraphs) {
	bool bAppear = pGraphs.empty() && nGraphs.empty();
	int scoreIndex = 0;
	int l = head < pred ? head : pred;
	int r = head > pred ? head : pred;
	if (goldGraph != cGraph) {
		++scoreIndex;
	}
	for (const auto & arc : cGraph[l].m_vecRightArcs) {
		if (arc.first == r) {
			++scoreIndex;
		}
	}
	for (const auto & graph : pGraphs) {
		for (const auto & arc : graph[l].m_vecRightArcs) {
			if (arc.first == r) {
				bAppear = true;
				++scoreIndex;
			}
		}
	}
	for (const auto & graph : nGraphs) {
		for (const auto & arc : graph[l].m_vecRightArcs) {
			if (arc.first == r) {
				--scoreIndex;
			}
		}
	}
	for (const auto & arc : goldGraph[l].m_vecRightArcs) {
		if (arc.first == r && bAppear) {
			return CORRECT_ARC_SCORE * scoreIndex;
		}
	}
	return ARC_SCORE(l, r, goldGraph.size());
}

ttoken PseudoTreeFitting::arcLabel(const int & head, const int & pred, const CoNLL08DepGraph & graph) {
	int l = head < pred ? head : pred;
	int r = head > pred ? head : pred;
	for (const auto & arc : graph[l].m_vecRightArcs) {
		if (arc.first == r) {
			if (IS_LEFT_LABEL(arc.second)) {
				return r == head ? ENCODE_FORE_LABEL(DECODE_LEFT_LABEL(arc.second)) : ENCODE_BACK_LABEL(DECODE_LEFT_LABEL(arc.second));
			}
			else if (IS_RIGHT_LABEL(arc.second)) {
				return l == head ? ENCODE_FORE_LABEL(DECODE_RIGHT_LABEL(arc.second)) : ENCODE_BACK_LABEL(DECODE_RIGHT_LABEL(arc.second));
			}
			else {
				return r == head ? ENCODE_FORE_LABEL(DECODE_TWOWAY_LEFT_LABEL(arc.second)) : ENCODE_FORE_LABEL(DECODE_TWOWAY_RIGHT_LABEL(arc.second));
			}
		}
	}
	return PSEUDO_LABEL;
}

std::vector<PseudoTreeFitting::tEisnerArc> PseudoTreeFitting::decodeEisnerArcs(const CoNLL08DepGraph & graph, const CoNLL08DepGraph & cGraph, const std::vector<CoNLL08DepGraph> & pGraphs, const std::vector<CoNLL08DepGraph> & nGraphs) {
	// decode
	std::vector<std::vector<State>> vecStates;
	vecStates.push_back(std::vector<State>());
	std::vector<State> uniStates;
	for (int i = 0, len = graph.size(); i < len; ++i) {
		uniStates.push_back(State(i, i));
	}
	vecStates.push_back(uniStates);
	for (int d = 2, len = graph.size(); d <= len + 1; ++d) {

		std::vector<State> levelStates;

		for (int i = 0, n = len - d + 1; i < n; ++i) {

			State item(i, i + d - 1);

			const int & l2r_arc_score = arcScore(i, i + d - 1, graph, cGraph, pGraphs, nGraphs);
			const int & r2l_arc_score = arcScore(i + d - 1, i, graph, cGraph, pGraphs, nGraphs);

			for (int s = i, j = i + d; s < j - 1; ++s) {

				int partial_im_complete_score = vecStates[s - i + 1][i].l2r.first + vecStates[j - s - 1][s + 1].r2l.first;

				item.updateL2RIm(partial_im_complete_score + l2r_arc_score, s);
				item.updateR2LIm(partial_im_complete_score + r2l_arc_score, s);
			}

			item.updateL2R(item.l2r_im.first, item.right);
			item.updateR2L(item.r2l_im.first, item.left);

			for (int s = i + 1, j = i + d; s < j - 1; ++s) {

				State & litem = vecStates[s - i + 1][i];
				State & ritem = vecStates[j - s][s];

				item.updateL2R(litem.l2r_im.first + ritem.l2r.first, s);
				item.updateR2L(ritem.r2l_im.first + litem.r2l.first, s);
			}

			levelStates.push_back(item);
		}

		State item(len - d + 1, len);

		item.updateR2LIm((d == 2 ? 0 : vecStates[d - 1][item.left].l2r.first) + 0, len - 1); // 0 is root arc score

		item.updateR2L(item.r2l_im.first, item.left);

		for (int i = item.left, s = item.left + 1, j = len + 1; s < j - 1; ++s) {
			item.updateR2L(vecStates[j - s][s].r2l_im.first + vecStates[s - i + 1][i].r2l.first, s);
		}

		levelStates.push_back(item);

		vecStates.push_back(levelStates);
	}

	// decode arcs
	typedef std::tuple<int, int, ttoken> tBound;
	int len = graph.size();
	std::vector<tEisnerArc> vecArcs;
	std::stack<tBound> stack;
	stack.push(tBound(len + 1, 0, "r2l"));

	while (!stack.empty()) {
		tBound span = stack.top();
		stack.pop();
		if (std::get<0>(span) == 1) {
			continue;
		}
		State & item = vecStates[std::get<0>(span)][std::get<1>(span)];

		if (strcmp(std::get<2>(span).c_str(), "l2r") == 0) {
			int & s = item.l2r.second;
			vecArcs.push_back(tEisnerArc(item.left, s, arcScore(item.left, s, graph, cGraph, pGraphs, nGraphs)));
			if (item.left < item.right - 1) {
				stack.push(tBound(s - item.left + 1, item.left, "l2r_im"));
				stack.push(tBound(item.right - s + 1, s, "l2r"));
			}
		}
		else if (strcmp(std::get<2>(span).c_str(), "r2l") == 0) {
			int & s = item.r2l.second;
			vecArcs.push_back(tEisnerArc(item.right == len ? -1 : item.right, s, item.right == len ? 0 : arcScore(item.right, s, graph, cGraph, pGraphs, nGraphs)));
			if (item.left < item.right - 1) {
				stack.push(tBound(item.right - s + 1, s, "r2l_im"));
				stack.push(tBound(s - item.left + 1, item.left, "r2l"));
			}
		}
		else if (strcmp(std::get<2>(span).c_str(), "l2r_im") == 0) {
			int & s = item.l2r_im.second;
			stack.push(tBound(s - item.left + 1, item.left, "l2r"));
			stack.push(tBound(item.right - s, s + 1, "r2l"));
		}
		else if (strcmp(std::get<2>(span).c_str(), "r2l_im") == 0) {
			int & s = item.r2l_im.second;
			stack.push(tBound(s - item.left + 1, item.left, "l2r"));
			stack.push(tBound(item.right - s, s + 1, "r2l"));
		}
	}

	return vecArcs;
}

DependencyTree PseudoTreeFitting::extractPseudoTree(const CoNLL08DepGraph & graph, const std::vector<CoNLL08DepGraph> & pGraphs, const std::vector<CoNLL08DepGraph> & nGraphs, bool keepPseudo) {
	DependencyTree tree;

	auto vecArcs = decodeEisnerArcs(graph, graph, pGraphs, nGraphs);

	// get tree
	for (const auto & node : graph) {
		tree.push_back(DependencyTreeNode(POSTaggedWord(node.m_sWord, node.m_sPOSTag), -1, ""));
	}
	for (const auto & arc : vecArcs) {
		TREENODE_HEAD(tree[std::get<1>(arc)]) = std::get<0>(arc);
		ttoken label = std::get<0>(arc) == -1 ? "PROOT" : arcLabel(std::get<0>(arc), std::get<1>(arc), graph);
		if (keepPseudo && std::get<0>(arc) != -1) {
			label = DECODE_FORE_LABEL(label);
		}
		TREENODE_LABEL(tree[std::get<1>(arc)]) = label;
	}
	return tree;
}

DependencyTree PseudoTreeFitting::extractPseudoTree(const CoNLL08DepGraph & graph, const CoNLL08DepGraph & cGraph, const std::vector<CoNLL08DepGraph> & pGraphs, const std::vector<CoNLL08DepGraph> & nGraphs, bool keepPseudo) {
	DependencyTree tree;

	auto vecArcs = decodeEisnerArcs(graph, cGraph, pGraphs, nGraphs);

	// get tree
	for (const auto & node : graph) {
		tree.push_back(DependencyTreeNode(POSTaggedWord(node.m_sWord, node.m_sPOSTag), -1, ""));
	}
	for (const auto & arc : vecArcs) {
		TREENODE_HEAD(tree[std::get<1>(arc)]) = std::get<0>(arc);
		ttoken label = std::get<0>(arc) == -1 ? "PROOT" : arcLabel(std::get<0>(arc), std::get<1>(arc), graph);
		if (keepPseudo && std::get<0>(arc) != -1) {
			label = DECODE_FORE_LABEL(label);
		}
		TREENODE_LABEL(tree[std::get<1>(arc)]) = label;
	}
	return tree;
}

DependencyPathTree PseudoTreeFitting::extractPseudoScoreTree(const CoNLL08DepGraph & graph, const std::vector<CoNLL08DepGraph> & pGraphs, const std::vector<CoNLL08DepGraph> & nGraphs, bool keepPseudo) {
	DependencyPathTree tree;

	auto vecArcs = decodeEisnerArcs(graph, graph, pGraphs, nGraphs);

	// get tree
	for (const auto & node : graph) {
		tree.push_back(DependencyPathTreeNode(DependencyTreeNode(POSTaggedWord(node.m_sWord, node.m_sPOSTag), -1, ""), {0}));
	}
	for (const auto & arc : vecArcs) {
		TREENODE_HEAD(tree[std::get<1>(arc)].first) = std::get<0>(arc);
		ttoken label = std::get<0>(arc) == -1 ? "PROOT" : arcLabel(std::get<0>(arc), std::get<1>(arc), graph);
		if (keepPseudo && std::get<0>(arc) != -1) {
			label = DECODE_FORE_LABEL(label);
		}
		TREENODE_LABEL(tree[std::get<1>(arc)].first) = label;
		tree[std::get<1>(arc)].second.front() = std::get<2>(arc);
	}
	return tree;
}

DependencyPathTree PseudoTreeFitting::extractPseudoScoreTree(const CoNLL08DepGraph & graph, const CoNLL08DepGraph & cGraph, const std::vector<CoNLL08DepGraph> & pGraphs, const std::vector<CoNLL08DepGraph> & nGraphs, bool keepPseudo) {
	DependencyPathTree tree;

	auto vecArcs = decodeEisnerArcs(graph, cGraph, pGraphs, nGraphs);

	// get tree
	for (const auto & node : graph) {
		tree.push_back(DependencyPathTreeNode(DependencyTreeNode(POSTaggedWord(node.m_sWord, node.m_sPOSTag), -1, ""), {0}));
	}
	for (const auto & arc : vecArcs) {
		TREENODE_HEAD(tree[std::get<1>(arc)].first) = std::get<0>(arc);
		ttoken label = std::get<0>(arc) == -1 ? "PROOT" : arcLabel(std::get<0>(arc), std::get<1>(arc), graph);
		if (keepPseudo && std::get<0>(arc) != -1) {
			label = DECODE_FORE_LABEL(label);
		}
		TREENODE_LABEL(tree[std::get<1>(arc)].first) = label;
		tree[std::get<1>(arc)].second.front() = std::get<2>(arc);
	}
	return tree;
}

CoNLL08DepGraph PseudoTreeFitting::pseudoTreeToGraph(const DependencyTree & tree, bool keepPseudo) {
	CoNLL08DepGraph pseudoGraph;

	std::vector<tEisnerArc> vecArcs;
	for (int i = 0, n = tree.size(); i < n; ++i) {
		vecArcs.push_back(tEisnerArc(TREENODE_HEAD(tree[i]), i, 0));
	}

	// get graph
	for (const auto & node : tree) {
		pseudoGraph.add(CoNLL08DepNode());
		pseudoGraph.back().m_sWord = TREENODE_WORD(node);
		pseudoGraph.back().m_sPOSTag = TREENODE_POSTAG(node);
		pseudoGraph.back().m_nTreeHead = TREENODE_HEAD(node);
		pseudoGraph.back().m_vecRightArcs.clear();
	}
	for (const auto & arc : vecArcs) {
		int h = std::get<0>(arc), p = std::get<1>(arc);
		if (h == -1) continue;
		ttoken label = TREENODE_LABEL(tree[p]);
		if (IS_FORE_LABEL(label)) {
			if (keepPseudo) {
				pseudoGraph[h < p ? h : p].m_vecRightArcs.push_back(
						std::pair<int, ttoken>(h > p ? h : p,
								h > p ? ENCODE_LEFT_LABEL(label) : ENCODE_RIGHT_LABEL(label)));
			}
			else {
				pseudoGraph[h < p ? h : p].m_vecRightArcs.push_back(
						std::pair<int, ttoken>(h > p ? h : p,
								h > p ? ENCODE_LEFT_LABEL(DECODE_FORE_LABEL(label)) : ENCODE_RIGHT_LABEL(DECODE_FORE_LABEL(label))));
			}
		}
		else if (IS_BACK_LABEL(label)) {
			if (keepPseudo) {
				pseudoGraph[h < p ? h : p].m_vecRightArcs.push_back(
						std::pair<int, ttoken>(h > p ? h : p,
								h > p ? ENCODE_LEFT_LABEL(label) : ENCODE_RIGHT_LABEL(label)));
			}
			else {
				pseudoGraph[h < p ? h : p].m_vecRightArcs.push_back(
						std::pair<int, ttoken>(h > p ? h : p,
								h < p ? ENCODE_LEFT_LABEL(DECODE_BACK_LABEL(label)) : ENCODE_RIGHT_LABEL(DECODE_BACK_LABEL(label))));
			}
		}
		else if (keepPseudo) {
			pseudoGraph[h < p ? h : p].m_vecRightArcs.push_back(
					std::pair<int, ttoken>(h > p ? h : p,
							h > p ? ENCODE_LEFT_LABEL(std::string("PSEUDO")) : ENCODE_RIGHT_LABEL(std::string("PSEUDO"))));
		}
	}
	for (auto && node : pseudoGraph) {
		std::sort(node.m_vecRightArcs.begin(), node.m_vecRightArcs.end(), [](const std::pair<int, ttoken> & arc1, const std::pair<int, ttoken> & arc2) {
			return arc1.first < arc2.first;
		});
	}
	return pseudoGraph;
}


CoNLL08DepGraph PseudoTreeFitting::addPseudoTag(const CoNLL08DepGraph & gold, const CoNLL08DepGraph & pseudo) {
	CoNLL08DepGraph graph = pseudo;
	std::unordered_set<int> addset;
	for (int i = 0; i < gold.size(); ++i) {
		for (const auto & arc : pseudo[i].m_vecRightArcs) {
			if (arc.second == "leftPSEUDO" || arc.second == "rightPSEUDO") {
				addset.insert(i);
				addset.insert(arc.first);
			}
		}
		for (const auto & arc : gold[i].m_vecRightArcs) {
			bool find = false;
			for (const auto & parc : pseudo[i].m_vecRightArcs) {
				if (arc.first == parc.first && arc.second != "leftPSEUDO" && arc.second != "rightPSEUDO") {
					find = true;
					break;
				}
			}
			if (!find) {
				addset.insert(i);
				addset.insert(arc.first);
			}
		}
	}
	for (int i = 0; i < gold.size(); ++i) {
		graph[i].m_sSuperTag = addset.find(i) == addset.end() ? "S" : "K";
	}
	return graph;
}
