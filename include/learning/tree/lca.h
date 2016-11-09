#ifndef _LCA_H
#define _LCA_H

#include "stdafx.h"

#include <string>

#include "common/parser/macros_base.h"

class LCA {
public:
	static std::string UP;
	static std::string DOWN;
	static std::string ROOT;
	static std::string NONE;

private:

	int size;
	int eular_size;
	int block_size;
	int block_count;

	int Clog2[MAX_SENTENCE_SIZE << 1];
	int Flog2[MAX_SENTENCE_SIZE << 1];

	int Eular[MAX_SENTENCE_SIZE << 1];
	int Depth[MAX_SENTENCE_SIZE << 1];
	int Index[MAX_SENTENCE_SIZE];

	int GroupMin[MAX_SENTENCE_SIZE << 1];
	int GroupIndex[MAX_SENTENCE_SIZE << 1];
	int GroupState[MAX_SENTENCE_SIZE << 1];

	bool BeState[MAX_SENTENCE_SIZE];
	int InGroup[MAX_SENTENCE_SIZE][MAX_SENTENCE_BITS][MAX_SENTENCE_BITS];
	int BetweenGroup[MAX_SENTENCE_SIZE][MAX_SENTENCE_BITS];

	int head[MAX_SENTENCE_SIZE];
	int pred[MAX_SENTENCE_SIZE];

	int ceillog2(int n);
	int floorlog2(int n);

	int query(int i, int j);
	void loadTree(std::vector<int> & heads);

public:

	LCA();
	~LCA() = default;

	std::vector<std::vector<std::string>> POSPath;
	std::vector<std::vector<std::string>> FPOSPath;

	void loadPath(DependencyTree & tree);

};

#endif
