#include "stdafx.h"

#include <stack>
#include <algorithm>

#include "dependency_primitive.h"

CoNLL08DepNode::CoNLL08DepNode() : m_sWord(""), m_sPOSTag(""), m_sSuperTag(NULL_SUPERTAG), m_nTreeHead(-1), m_nSuperTagCode(0) {}

CoNLL08DepNode::~CoNLL08DepNode() = default;

CoNLL08DepNode::CoNLL08DepNode(const CoNLL08DepNode & node) :
		m_sWord(node.m_sWord), m_sPOSTag(node.m_sPOSTag), m_sSuperTag(node.m_sSuperTag), m_nTreeHead(node.m_nTreeHead), m_nSuperTagCode(node.m_nSuperTagCode),
		m_vecRightArcs(node.m_vecRightArcs), m_vecRightLabels(node.m_vecRightLabels) {}

bool operator==(const CoNLL08DepNode & n1, const CoNLL08DepNode & n2) {
	return n1.m_sWord == n2.m_sWord && n1.m_sPOSTag == n2.m_sPOSTag && n1.m_nTreeHead == n2.m_nTreeHead &&
			n1.m_vecRightArcs == n2.m_vecRightArcs;
}

bool operator!=(const CoNLL08DepNode & n1, const CoNLL08DepNode & n2) {
	return !(n1 == n2);
}

CoNLL08DepGraph::CoNLL08DepGraph() = default;

CoNLL08DepGraph::~CoNLL08DepGraph() = default;

CoNLL08DepGraph::CoNLL08DepGraph(const CoNLL08DepGraph & graph) : m_vecNodes(graph.m_vecNodes) {}

// add crossing edge
CoNLL08DepGraph & CoNLL08DepGraph::operator=(const CoNLL08DepGraph & g) {
	m_vecNodes = g.m_vecNodes;
	return *this;
}

// add crossing edge
CoNLL08DepGraph & CoNLL08DepGraph::operator+=(const CoNLL08DepGraph & g) {
	for (int i = 0, n = size(); i < n; ++i) {
		for (const auto & arc : g[i].m_vecRightArcs) {
			// j < i < a.first < arc.first
			for (int j = 0; j < i; ++j) {
				for (const auto & a : m_vecNodes[j].m_vecRightArcs) {
					if (i < a.first && a.first < arc.first) {
						m_vecNodes[i].m_vecRightArcs.push_back(arc);
					}
				}
			}
			// i < j < arc.first < a.first
			for (int j = i + 1; j < arc.first; ++j) {
				for (const auto & a : m_vecNodes[j].m_vecRightArcs) {
					if (arc.first < a.first) {
						m_vecNodes[i].m_vecRightArcs.push_back(arc);
					}
				}
			}
		}
		std::sort(m_vecNodes[i].m_vecRightArcs.begin(), m_vecNodes[i].m_vecRightArcs.end(),
				[](const std::pair<int, ttokenold> & p1, const std::pair<int, ttokenold> & p2) { return p1.first < p2.first; });
	}
	return *this;
}

// reverse
CoNLL08DepGraph CoNLL08DepGraph::operator-() {
	CoNLL08DepGraph graph;
	for (auto && itr = m_vecNodes.rbegin(); itr != m_vecNodes.rend(); ++itr) {
		graph.add(*itr);
		graph.back().m_vecRightArcs.clear();
		graph.back().m_nTreeHead = itr->m_nTreeHead == -1 ? -1 : m_vecNodes.size() - itr->m_nTreeHead - 1;
	}
	for (int i = 0, n = size(); i < n; ++i) {
		for (const auto & arc : m_vecNodes[i].m_vecRightArcs) {
			graph[n - arc.first - 1].m_vecRightArcs.push_back(std::pair<int, ttokenold>(n - i - 1,
					IS_LEFT_LABEL(arc.second) ? ENCODE_RIGHT_LABEL(DECODE_LEFT_LABEL(arc.second)) :
							IS_RIGHT_LABEL(arc.second) ? ENCODE_LEFT_LABEL(DECODE_RIGHT_LABEL(arc.second)) :
									ENCODE_TWOWAY_LABEL(DECODE_TWOWAY_RIGHT_LABEL(arc.second), DECODE_TWOWAY_LEFT_LABEL(arc.second))));
		}
	}
	for (auto && node : graph) {
		std::sort(node.m_vecRightArcs.begin(), node.m_vecRightArcs.end(),
				[](const std::pair<int, ttokenold> & p1, const std::pair<int, ttokenold> & p2) { return p1.first < p2.first; });
	}
	return graph;
}

std::pair<CoNLL08DepGraph, CoNLL08DepGraph> CoNLL08DepGraph::splitPlanar(bool planar) {
	std::pair<CoNLL08DepGraph, CoNLL08DepGraph> parts(*this, *this);
	std::unordered_map<BiGram<int>, int> arcs;
	std::unordered_set<BiGram<int>> removed;
	// record all crossing edges from parts
	for (int i = 0, n = size(); i < n; ++i) {
		for (const auto & arc : parts.second[i].m_vecRightArcs) {
			for (int j = 0; j < n; ++j) {
				for (const auto & sarc : parts.second[j].m_vecRightArcs) {
					// crossing with i < j < right(i) < right(j)
					if (i < j && j < arc.first && arc.first < sarc.first) {
						arcs[BiGram<int>(i, std::get<0>(arc))] += 1;
					}
					// crossing with j < i < right(j) < right(i)
					else if (j < i && i < sarc.first && sarc.first < arc.first) {
						arcs[BiGram<int>(i, std::get<0>(arc))] += 1;
					}
				}
			}
		}
	}
	// remove all crossing edges from parts
	for (const auto & arc : arcs) {
		auto & rightArcs = (parts.second)[arc.first.first()].m_vecRightArcs;
		for (auto && itr = rightArcs.begin(); itr != rightArcs.end(); ++itr) {
			if (itr->first == arc.first.second()) {
				rightArcs.erase(itr);
				break;
			}
		}
	}
	while (true) {
		// save all crossing edges
		arcs.clear();
		for (int i = 0, n = size(); i < n; ++i) {
			for (const auto & arc : parts.first[i].m_vecRightArcs) {
				for (int j = 0; j < n; ++j) {
					for (const auto & sarc : parts.first[j].m_vecRightArcs) {
						// crossing with i < j < right(i) < right(j)
						if (i < j && j < arc.first && arc.first < sarc.first) {
							arcs[BiGram<int>(i, std::get<0>(arc))] += 1;
						}
						// crossing with j < i < right(j) < right(i)
						else if (j < i && i < sarc.first && sarc.first < arc.first) {
							arcs[BiGram<int>(i, std::get<0>(arc))] += 1;
						}
					}
				}
			}
		}
		if (arcs.size() == 0) {
			break;
		}
		// find most-corssing edge
		int maxCount = arcs.begin()->second;
		BiGram<int> maxArc(arcs.begin()->first);
		for (const auto & arc : arcs) {
			if (arc.second > maxCount) {
				maxCount = arc.second;
				maxArc = arc.first;
			}
			else if (arc.second == maxCount) {
				for (auto && ra : removed) {
					if (!(ra.first() < arc.first.first() && arc.first.first() < ra.second() && ra.second() < arc.first.second()) &&
						!(arc.first.first() < ra.first() && ra.first() < arc.first.second() && arc.first.second() < ra.second())) {
						maxArc = arc.first;
					}
				}
			}
		}
		removed.insert(maxArc);
		auto & rightArcs = parts.second[maxArc.first()].m_vecRightArcs;
		auto & deleteNode = parts.first[maxArc.first()].m_vecRightArcs;
		// remove max-arc
		for (auto itr = deleteNode.begin(); itr != deleteNode.end(); ++itr) {
			if (itr->first == maxArc.second()) {
				// add before delete, avoiding iterator error
				// add maxArc to parts.second
				// add only no-crossing
				bool bNoCrossing = true;
				// left is maxArc.first(), right is maxArc.second()
				for (int i = 0, j = maxArc.first(); i < j && bNoCrossing; ++i) {
					for (const auto & arc : parts.second[i].m_vecRightArcs) {
						if (j < arc.first && arc.first < maxArc.second()) {
							bNoCrossing = false;
							break;
						}
					}
				}
				for (int j = maxArc.first(), i = j + 1, n = m_vecNodes.size(); i < n && bNoCrossing; ++i) {
					for (const auto & arc : parts.second[i].m_vecRightArcs) {
						if (i < maxArc.second() && maxArc.second() < arc.first) {
							bNoCrossing = false;
							break;
						}
					}
				}
				if (!planar || bNoCrossing) {
					rightArcs.push_back(*itr);
					std::sort(rightArcs.begin(), rightArcs.end(),
							[](const std::pair<int, ttokenold> & p1, const std::pair<int, ttokenold> & p2) { return p1.first < p2.first; });
				}
				// remove maxArc from parts.first
				deleteNode.erase(itr);
				break;
			}
		}
	}
	return parts;
}

void CoNLL08DepGraph::setTagsAndLabels(const TokenOld & labels, const TokenOld & supertags, const std::vector<int> & vecLabels) {
	for (auto && node : m_vecNodes) {
		node.m_nSuperTagCode = supertags.find(node.m_sSuperTag);
		node.m_vecRightLabels.clear();
		for (const auto & arc : node.m_vecRightArcs) {
			const int & label = labels.find(arc.second);
			node.m_vecRightLabels.push_back(
					std::pair<int, std::pair<int, int>>
					(label, std::pair<int, int>(LEFT_LABEL_ID(vecLabels[label]), RIGHT_LABEL_ID(vecLabels[label]))));
		}
	}
}

std::vector<std::vector<int>> CoNLL08DepGraph::stronglyComponet(bool connected) {
	int n = m_vecNodes.size();
	std::vector<std::vector<int>> components;
	// tarjan
	int nTime = 0;
	std::stack<int> stack;
	std::vector<bool> inStack(n, false);
	std::vector<int> vecDFN(n, -1), vecLow(n, -1);
	std::vector<std::vector<int>> vecChildren(n);
	// init children
	for (int i = 0; i < n; ++i) {
		for (const auto & arc : m_vecNodes[i].m_vecRightArcs) {
			if (connected) {
				if (IS_LEFT_LABEL(arc.second)) {
					vecChildren[arc.first].push_back(i);
				}
				else if (IS_RIGHT_LABEL(arc.second)) {
					vecChildren[i].push_back(arc.first);
				}
				else if (IS_TWOWAY_LABEL(arc.second)) {
					vecChildren[arc.first].push_back(i);
					vecChildren[i].push_back(arc.first);
				}
			}
			else {
				vecChildren[arc.first].push_back(i);
				vecChildren[i].push_back(arc.first);
			}
		}
	}
	// search
	for (int i = 0; i < n; ++i) {
		typedef std::pair<int, int> Info;
		std::stack<Info> pseudoRecursion;
		if (vecDFN[i] == -1) {
			pseudoRecursion.push(Info(i, -1));
			while (!pseudoRecursion.empty()) {
				Info info = pseudoRecursion.top();
				pseudoRecursion.pop();
				int v = info.first;
				std::vector<int>& vecLink = vecChildren[v];
				// first visit
				if (vecDFN[v] == -1) {
					vecDFN[v] = vecLow[v] = nTime++;
					stack.push(v);
					inStack[v] = true;
				}
				// from last recursion
				if (info.second != -1) {
					vecLow[v] = std::min(vecLow[v], vecLow[vecLink[info.second]]);
				}
				bool nextRecursion = false;
				for (int j = info.second + 1, max_j = vecLink.size(); j < max_j; ++j) {
					int& link = vecLink[j];
					if (vecDFN[link] == -1) {
						pseudoRecursion.push(Info(v, j));
						pseudoRecursion.push(Info(link, -1));
						nextRecursion = true;
						break;
					}
					else if (inStack[link]) {
						vecLow[v] = std::min(vecLow[v], vecDFN[link]);
					}
				}
				if (nextRecursion) {
					continue;
				}
				else if (vecDFN[v] == vecLow[v]) {
					std::vector<int> component;
					do {
						v = stack.top();
						stack.pop();
						inStack[v] = false;
						component.push_back(v);
					} while (vecDFN[v] != vecLow[v]);
					components.push_back(component);
				}
			}
		}
	}
	return components;
}

std::vector<std::vector<std::vector<int>>> CoNLL08DepGraph::shortestPaths(bool connected) {
	int n = m_vecNodes.size();
	std::vector<std::vector<std::vector<int>>> vecShortestPaths(n, std::vector<std::vector<int>>(n, std::vector<int>()));
	auto components = connected ? std::vector<std::vector<int>>(1) : stronglyComponet();
	if (components.size() == 1) {
		components.front().clear();
		for (int i = 0; i < n; ++i) {
			components.front().push_back(i);
		}
	}
	std::vector<std::vector<int>> vecChildren(n);
	// init children
	for (int i = 0; i < n; ++i) {
		for (const auto & arc : m_vecNodes[i].m_vecRightArcs) {
			if (connected) {
				if (IS_LEFT_LABEL(arc.second)) {
					vecChildren[arc.first].push_back(i);
				}
				else if (IS_RIGHT_LABEL(arc.second)) {
					vecChildren[i].push_back(arc.first);
				}
				else if (IS_TWOWAY_LABEL(arc.second)) {
					vecChildren[arc.first].push_back(i);
					vecChildren[i].push_back(arc.first);
				}
			}
			else {
				vecChildren[arc.first].push_back(i);
				vecChildren[i].push_back(arc.first);
			}
		}
	}
	for (const auto & component : components) {
		for (const auto & v : component) {
			std::vector<bool> vecVisit(n, false);
			std::vector<int> candidates;
			candidates.push_back(v);
			vecShortestPaths[v][v] = std::vector<int>(1, v);
			vecVisit[v] = true;
			while (!candidates.empty()) {
				std::vector<int> temp;
				for (const auto & c : candidates) {
					for (const auto & l : vecChildren[c]) {
						if (!vecVisit[l]) {
							temp.push_back(l);
							auto path = vecShortestPaths[v][c];
							path.push_back(l);
							vecShortestPaths[v][l] = path;
							vecVisit[l] = true;
						}
					}
				}
				candidates = temp;
			}
		}
	}
	return vecShortestPaths;
}

std::string CoNLL08DepGraph::labelPath(std::vector<int>& path, std::string type) {
	if (path.empty() || path.size() == 1) return "#n";
	if (path.size() > 4) return "#l";
	std::string result;
	if (type == "pos") result = m_vecNodes[path[0]].m_sPOSTag;
	else if (type == "fpos") result = m_vecNodes[path[0]].m_sPOSTag.substr(0, 1);
	for (int i = 1, n = path.size(); i < n; ++i) {
		int l = path[i - 1], r = path[i];
		if (l > r) std::swap(l, r);
		std::string label;
		for (const auto & arc : m_vecNodes[l].m_vecRightArcs) {
			if (arc.first == r) {
				label = arc.second;
				break;
			}
		}
		if (IS_LEFT_LABEL(label)) {
			if (type == "label")
				label = DECODE_LEFT_LABEL(label);
			else if (type == "pos")
				label = m_vecNodes[path[i]].m_sPOSTag;
			else if (type == "fpos")
				label = m_vecNodes[path[i]].m_sPOSTag.substr(0, 1);
			if (path[i] == r) label = "r" + label;
		}
		else if (IS_RIGHT_LABEL(label)) {
			if (type == "label")
				label = DECODE_RIGHT_LABEL(label);
			else if (type == "pos")
				label = m_vecNodes[path[i]].m_sPOSTag;
			else if (type == "fpos")
				label = m_vecNodes[path[i]].m_sPOSTag.substr(0, 1);
			if (path[i] == l) label = "r" + label;
		}
		else if (IS_TWOWAY_LABEL(label)) {
			if (type == "label") {
				if (path[i] == r) label = DECODE_TWOWAY_RIGHT_LABEL(label);
				else label = DECODE_TWOWAY_LEFT_LABEL(label);
			}
			else if (type == "pos")
				label = m_vecNodes[path[i]].m_sPOSTag;
			else if (type == "fpos")
				label = m_vecNodes[path[i]].m_sPOSTag.substr(0, 1);
		}
		result = result + "#" + label;
	}
	return result;
}

CoNLL08DepGraph CoNLL08DepGraph::treeOrderGraph() {
	int len = size();
	CoNLL08DepGraph graph;
	std::vector<int> treeOrder;
	std::vector<int> treeOrderInverse;
	std::vector<std::set<int>> childrens;
	typedef std::tuple<int, int, ttokenold> tArc;
	std::vector<tArc> arcs;
	for (int i = 0; i < len; ++i) {
		childrens.push_back(std::set<int>());
		treeOrderInverse.push_back(0);
		for (const auto & arc : m_vecNodes[i].m_vecRightArcs) {
			arcs.push_back(tArc(i, arc.first, arc.second));
		}
	}
	childrens.push_back(std::set<int>());
	for (int i = 0; i < len; ++i) {
		if (m_vecNodes[i].m_nTreeHead != -1) {
			childrens[m_vecNodes[i].m_nTreeHead].insert(i);
		}
		else {
			childrens[len].insert(i);
		}
	}
	// most-children firsts
	int pivot = len;
	while (!childrens[len].empty()) {
		if (childrens[pivot].empty()) {
			treeOrder.push_back(pivot);
			int head = m_vecNodes[pivot].m_nTreeHead;
			if (head == -1) head = len;
			childrens[head].erase(pivot);
			pivot = head;
		}
		else {
			int newPivot = *childrens[pivot].begin();
			for (const auto & child : childrens[pivot]) {
				if (childrens[child].size() >= childrens[newPivot].size()) {
					newPivot = child;
				}
			}
			pivot = newPivot;
		}
	}
	for (int i = 0; i < len; ++i) {
		treeOrderInverse[treeOrder[i]] = i;
	}
	for (auto && arc : arcs) {
		std::get<0>(arc) = treeOrderInverse[std::get<0>(arc)];
		std::get<1>(arc) = treeOrderInverse[std::get<1>(arc)];
		if (std::get<0>(arc) > std::get<1>(arc)) {
			std::swap(std::get<0>(arc), std::get<1>(arc));
			std::get<2>(arc) =
					IS_LEFT_LABEL(std::get<2>(arc)) ? ENCODE_RIGHT_LABEL(DECODE_LEFT_LABEL(std::get<2>(arc))) :
							IS_RIGHT_LABEL(std::get<2>(arc)) ? ENCODE_LEFT_LABEL(DECODE_RIGHT_LABEL(std::get<2>(arc))) :
									ENCODE_TWOWAY_LABEL(DECODE_TWOWAY_RIGHT_LABEL(std::get<2>(arc)), DECODE_TWOWAY_LEFT_LABEL(std::get<2>(arc)));
		}
	}
	std::sort(arcs.begin(), arcs.end(), [](const tArc & arc1, const tArc & arc2) {
		if (std::get<0>(arc1) != std::get<0>(arc2))
			return std::get<0>(arc1) < std::get<0>(arc2);
		else
			return std::get<1>(arc1) < std::get<1>(arc2);
	});
	for (int i = 0; i < len; ++i) {
		graph.add(m_vecNodes[treeOrder[i]]);
		auto & node = graph.back();
		node.m_nTreeHead = graph.back().m_nTreeHead == -1 ? -1 : treeOrderInverse[node.m_nTreeHead];
		node.m_vecRightArcs.clear();
		for (const auto & arc : arcs) {
			if (std::get<0>(arc) == i) {
				node.m_vecRightArcs.push_back(std::pair<int, ttokenold>(std::get<1>(arc), std::get<2>(arc)));
			}
		}
	}
	return graph;
}


bool CoNLL08DepGraph::checkArc(const CoNLL08DepGraph & g) {
	std::set<std::string> arcs1, arcs2;
	for (const auto & node : m_vecNodes) {
		for (const auto & arc : node.m_vecRightArcs) {
			if (IS_LEFT_LABEL(arc.second)) {
				arcs1.insert(m_vecNodes[arc.first].m_sWord + DECODE_LEFT_LABEL(arc.second) + node.m_sWord);
			}
			else if (IS_RIGHT_LABEL(arc.second)) {
				arcs1.insert(node.m_sWord + DECODE_RIGHT_LABEL(arc.second) + m_vecNodes[arc.first].m_sWord);
			}
			else {
				arcs1.insert(m_vecNodes[arc.first].m_sWord + DECODE_TWOWAY_LEFT_LABEL(arc.second) + node.m_sWord);
				arcs1.insert(node.m_sWord + DECODE_TWOWAY_RIGHT_LABEL(arc.second) + m_vecNodes[arc.first].m_sWord);
			}
		}
	}
	for (const auto & node : g.m_vecNodes) {
		for (const auto & arc : node.m_vecRightArcs) {
			if (IS_LEFT_LABEL(arc.second)) {
				arcs2.insert(g.m_vecNodes[arc.first].m_sWord + DECODE_LEFT_LABEL(arc.second) + node.m_sWord);
			}
			else if (IS_RIGHT_LABEL(arc.second)) {
				arcs2.insert(node.m_sWord + DECODE_RIGHT_LABEL(arc.second) + g.m_vecNodes[arc.first].m_sWord);
			}
			else {
				arcs2.insert(g.m_vecNodes[arc.first].m_sWord + DECODE_TWOWAY_LEFT_LABEL(arc.second) + node.m_sWord);
				arcs2.insert(node.m_sWord + DECODE_TWOWAY_RIGHT_LABEL(arc.second) + g.m_vecNodes[arc.first].m_sWord);
			}
		}
	}
	bool equal = true;
	for (const auto arc : arcs1) {
		if (arcs2.find(arc) == arcs2.end()) {
			equal = false;
		}
	}
	for (const auto arc : arcs2) {
		if (arcs1.find(arc) == arcs1.end()) {
			equal = false;
		}
	}
	return equal;
}

// equal
bool operator==(const CoNLL08DepGraph & g1, const CoNLL08DepGraph & g2) {
	if (g1.size() != g2.size()) {
		return false;
	}
	for (int i = 0, n = g1.size(); i < n; ++i) {
		if (g1[i] != g2[i]) {
			return false;
		}
	}
	return true;
}

bool operator!=(const CoNLL08DepGraph & g1, const CoNLL08DepGraph & g2) {
	return !(g1 == g2);
}

// combine two graph
CoNLL08DepGraph operator+(const CoNLL08DepGraph & g1, const CoNLL08DepGraph & g2) {
	CoNLL08DepGraph graph;
	for (int i = 0, n = g1.size(); i < n; ++i) {
		graph.add(g1[i]);
		graph[i].m_vecRightArcs.clear();
		auto itr1 = g1[i].m_vecRightArcs.begin(), itr2 = g2[i].m_vecRightArcs.begin();
		auto bound1 = g1[i].m_vecRightArcs.end(), bound2 = g2[i].m_vecRightArcs.end();
		while (itr1 < bound1 && itr2 < bound2) {
			if (itr1->first < itr2->first) {
				graph[i].m_vecRightArcs.push_back(*itr1++);
			}
			else if (itr1->first > itr2->first) {
				graph[i].m_vecRightArcs.push_back(*itr2++);
			}
			else {
				if (itr1->second != itr2->second) {
					if (IS_LEFT_LABEL(itr1->second)) {
						if (IS_RIGHT_LABEL(itr2->second)) {
							graph[i].m_vecRightArcs.push_back(
								std::pair<int, ttokenold>(itr1->first,
								ENCODE_TWOWAY_LABEL(DECODE_LEFT_LABEL(itr1->second), DECODE_RIGHT_LABEL(itr2->second))));
						}
						else {
							graph[i].m_vecRightArcs.push_back(*itr1);
						}
					}
					else if (IS_RIGHT_LABEL(itr1->second)) {
						if (IS_LEFT_LABEL(itr2->second)) {
							graph[i].m_vecRightArcs.push_back(
								std::pair<int, ttokenold>(itr1->first,
								ENCODE_TWOWAY_LABEL(DECODE_LEFT_LABEL(itr2->second), DECODE_RIGHT_LABEL(itr1->second))));
						}
						else {
							graph[i].m_vecRightArcs.push_back(*itr1);
						}
					}
					else {
						graph[i].m_vecRightArcs.push_back(*itr1);
					}
				}
				else {
					graph[i].m_vecRightArcs.push_back(*itr1);
				}
				++itr1;
				++itr2;
			}
		}
		while (itr1 < bound1) {
			graph[i].m_vecRightArcs.push_back(*itr1++);
		}
		while (itr2 < bound2) {
			graph[i].m_vecRightArcs.push_back(*itr2++);
		}
	}
	return graph;
}

// remove arcs
CoNLL08DepGraph operator-(const CoNLL08DepGraph & g1, const CoNLL08DepGraph & g2) {
	CoNLL08DepGraph graph;
	for (int i = 0, n = g1.size(); i < n; ++i) {
		graph.add(g1[i]);
		graph.back().m_sSuperTag = g2[i].m_sSuperTag;
		for (const auto & arc : g2[i].m_vecRightArcs) {
			for (auto itr = graph.back().m_vecRightArcs.begin(); itr != graph.back().m_vecRightArcs.end(); ++itr) {
				if (itr->first == arc.first) {
					graph.back().m_vecRightArcs.erase(itr);
					break;
				}
			}
		}
	}
	return graph;
}

std::istream & operator>>(std::istream & is, CoNLL08DepGraph & graph) {
	ttokenold line, token;
	std::vector<int> heads;
	typedef std::tuple<int, int, ttokenold> tArcs;
	std::vector<tArcs> arcs;

	graph.clear();
	while (true) {
		std::getline(is, line);
		if (line.empty()) {
			break;
		}
		CoNLL08DepNode node;
		std::istringstream issLine(line);
		issLine >> token >> node.m_sWord >> token >> node.m_sPOSTag >> token >> token >> token >> node.m_sSuperTag >> token;
		if (token == "_") {
			node.m_nTreeHead = -1;
		}
		else {
			std::istringstream issToken(token);
			issToken >> node.m_nTreeHead;
		}
		issLine >> token >> token;
		if (token != "_") {
			heads.push_back(graph.size());
		}
		int i = 0;
		std::vector<std::tuple<int, ttokenold>> rightArcs;
		while (issLine >> token) {
			if (token != "_") {
				if (i < heads.size() && heads[i] != graph.size()) {
					arcs.push_back(tArcs(i, graph.size(), ENCODE_RIGHT_LABEL(token)));
				}
				else if (i >= heads.size()) {
					arcs.push_back(tArcs(graph.size(), i, ENCODE_LEFT_LABEL(token)));
				}
			}
			++i;
		}
		graph.add(node);
	}
	for (auto && arc : arcs) {
		if (IS_LEFT_LABEL(std::get<2>(arc))) {
			std::get<1>(arc) = heads[std::get<1>(arc)];
		}
		else {
			std::get<0>(arc) = heads[std::get<0>(arc)];
		}
	}
	std::sort(arcs.begin(), arcs.end(), [](const tArcs & arc1, const tArcs & arc2) {
		if (std::get<0>(arc1) != std::get<0>(arc2)) {
			return std::get<0>(arc1) < std::get<0>(arc2);
		}
		else if (std::get<1>(arc1) != std::get<1>(arc2)) {
			return std::get<1>(arc1) < std::get<1>(arc2);
		}
		else {
			return IS_LEFT_LABEL(std::get<2>(arc1));
		}
	});
	for (const auto & arc : arcs) {
		auto & rightArcs = graph[std::get<0>(arc)].m_vecRightArcs;
		if (rightArcs.size() > 0 && rightArcs.back().first == std::get<1>(arc)) {
			rightArcs.back().second = ENCODE_TWOWAY_LABEL(DECODE_LEFT_LABEL(rightArcs.back().second), DECODE_RIGHT_LABEL(std::get<2>(arc)));
		}
		else {
			rightArcs.push_back(std::pair<int, ttokenold>(std::get<1>(arc), std::get<2>(arc)));
		}
	}
	return is;
}

std::ostream & operator<<(std::ostream & os, const CoNLL08DepGraph & graph) {
	std::set<int> heads;
	std::map<int, std::map<int, ttokenold>> arcs;
	int i = 0;
	for (const auto & node : graph) {
		for (const auto & arc : node.m_vecRightArcs) {
			if (IS_LEFT_LABEL(arc.second)) {
				heads.insert(arc.first);
				arcs[i][arc.first] = DECODE_LEFT_LABEL(arc.second);
			}
			else if (IS_RIGHT_LABEL(arc.second)) {
				heads.insert(i);
				arcs[arc.first][i] = DECODE_RIGHT_LABEL(arc.second);
			}
			else {
				heads.insert(arc.first);
				heads.insert(i);
				arcs[i][arc.first] = DECODE_TWOWAY_LEFT_LABEL(arc.second);
				arcs[arc.first][i] = DECODE_TWOWAY_RIGHT_LABEL(arc.second);
			}
		}
		++i;
	}
	i = 0;
	std::vector<int> headOrders;
	for (const auto & head : heads) {
		headOrders.push_back(head);
	}
	for (const auto & node : graph.m_vecNodes) {
		os << i + 1 << " " << node.m_sWord << " " << node.m_sWord << " " << node.m_sPOSTag << " " << node.m_sPOSTag << " _ _ " << node.m_sSuperTag << " " << node.m_nTreeHead << " _ " << (heads.find(i) == heads.end() ? "_" : node.m_sWord);
		for (int j = 0; j < heads.size(); ++j) {
			os << " " << (arcs.find(i) == arcs.end() || arcs[i].find(headOrders[j]) == arcs[i].end() ? "_" : arcs[i][headOrders[j]]);
		}
		os << std::endl;
		++i;
	}
	os << std::endl;
	return os;
}
