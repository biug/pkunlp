#ifndef _DEPENDENCY_PRIMITIVE_H
#define _DEPENDENCY_PRIMITIVE_H

#include "stdafx.h"

#include <set>
#include <map>
#include <cstring>
#include <sstream>
#include <algorithm>
#include <unordered_set>
#include <unordered_map>

#include "ngram.h"
#include "common/oldtoken/token.h"

#define NULL_SUPERTAG	"_"

#define MAX_LABEL_ID			256
#define MAX_LABEL_ID_BITS		8
#define LEFT_LABEL_ID(LI)		((LI) >> MAX_LABEL_ID_BITS)
#define RIGHT_LABEL_ID(LI)		((LI) & (MAX_LABEL_ID - 1))
#define ENCODE_LABEL_ID(LL,LR)	(((LL) << MAX_LABEL_ID_BITS) | (LR))

#define IS_LEFT_LABEL(L)	(L.find("left") == 0)
#define IS_RIGHT_LABEL(L)	(L.find("right") == 0)
#define IS_TWOWAY_LABEL(L)	(L.find("twoway") == 0)

#define ENCODE_LEFT_LABEL(L)		("left" + L)
#define ENCODE_RIGHT_LABEL(L)		("right" + L)
#define ENCODE_TWOWAY_LABEL(L1,L2)	("twoway" + L1 + "||" + L2)

#define DECODE_LEFT_LABEL(L)			(L.substr(strlen("left")))
#define DECODE_RIGHT_LABEL(L)			(L.substr(strlen("right")))
#define DECODE_TWOWAY_LEFT_LABEL(L)		(L.substr(strlen("twoway"), L.find("||") - strlen("twoway")))
#define DECODE_TWOWAY_RIGHT_LABEL(L)	(L.substr(L.find("||") + strlen("||")))

struct DepRightArc {
	int head;
	int label;

	DepRightArc(const int & h, const int & l) : head(h), label(l) {}
	~DepRightArc() = default;
	DepRightArc(const DepRightArc & arc) : head(arc.head), label(arc.label) {}
};

struct CoNLL08DepNode {
	ttokenold m_sWord;
	ttokenold m_sPOSTag;
	ttokenold m_sSuperTag;
	int m_nTreeHead;
	int m_nSuperTagCode;

	std::vector<std::pair<int, ttokenold>> m_vecRightArcs;
	std::vector<std::pair<int, std::pair<int, int>>> m_vecRightLabels;

	CoNLL08DepNode();
	~CoNLL08DepNode();
	CoNLL08DepNode(const CoNLL08DepNode & node);
};

class CoNLL08DepGraph {
	typedef	std::vector<CoNLL08DepNode>::iterator		iterator;
	typedef std::vector<CoNLL08DepNode>::const_iterator	const_iterator;
private:
	std::vector<CoNLL08DepNode> m_vecNodes;

public:
	CoNLL08DepGraph();
	~CoNLL08DepGraph();
	CoNLL08DepGraph(const CoNLL08DepGraph & graph);

	void clear() { m_vecNodes.clear(); }
	const int size() const { return m_vecNodes.size(); }
	void add(const CoNLL08DepNode & node) { m_vecNodes.push_back(node); }
	void clearSuperTag() { for (auto && node : m_vecNodes) { node.m_sSuperTag = NULL_SUPERTAG; node.m_nSuperTagCode = 0; } }
	CoNLL08DepNode & back() { return m_vecNodes.back(); }
	CoNLL08DepNode & operator[](const int & i) { return m_vecNodes[i]; }
	const CoNLL08DepNode & operator[](const int & i) const { return m_vecNodes[i]; }

	iterator begin() { return m_vecNodes.begin(); }
	iterator end() { return m_vecNodes.end(); }
	const_iterator begin() const { return m_vecNodes.cbegin(); }
	const_iterator end() const { return m_vecNodes.cend(); }

	CoNLL08DepGraph & operator=(const CoNLL08DepGraph & g);
	CoNLL08DepGraph & operator+=(const CoNLL08DepGraph & g);
	CoNLL08DepGraph operator-();
	std::pair<CoNLL08DepGraph, CoNLL08DepGraph> splitPlanar(bool planar = true);
	void setTagsAndLabels(const TokenOld & labels, const TokenOld & supertags, const std::vector<int> & vecLabels);

	CoNLL08DepGraph treeOrderGraph();
	bool checkArc(const CoNLL08DepGraph & g);
	std::vector<std::vector<int>> stronglyComponet(bool connected = false);
	std::vector<std::vector<std::vector<int>>> shortestPaths(bool connected = false);
	std::string labelPath(std::vector<int>& path, std::string type);

	friend bool operator==(const CoNLL08DepGraph & g1, const CoNLL08DepGraph & g2);
	friend bool operator!=(const CoNLL08DepGraph & g1, const CoNLL08DepGraph & g2);
	friend CoNLL08DepGraph operator+(const CoNLL08DepGraph & g1, const CoNLL08DepGraph & g2);
	friend CoNLL08DepGraph operator-(const CoNLL08DepGraph & g1, const CoNLL08DepGraph & g2);
	friend std::istream & operator>>(std::istream & is, CoNLL08DepGraph & graph);
	friend std::ostream & operator<<(std::ostream & os, const CoNLL08DepGraph & graph);
};

//struct CoNLL08DepTreeOrderGraph {
//	CoNLL08DepGraph m_gGraph;
//	CoNLL08DepGraph m_gTreeOrderGraph;
//
//	CoNLL08DepTreeOrderGraph();
//	~CoNLL08DepTreeOrderGraph();
//	CoNLL08DepTreeOrderGraph(const CoNLL08DepTreeOrderGraph & gs);
//
//	void readTree
//};

bool operator==(const CoNLL08DepGraph & g1, const CoNLL08DepGraph & g2);
bool operator!=(const CoNLL08DepGraph & g1, const CoNLL08DepGraph & g2);
CoNLL08DepGraph operator+(const CoNLL08DepGraph & g1, const CoNLL08DepGraph & g2);
std::istream & operator>>(std::istream & is, CoNLL08DepGraph & graph);
std::ostream & operator<<(std::ostream & os, const CoNLL08DepGraph & graph);

#endif
