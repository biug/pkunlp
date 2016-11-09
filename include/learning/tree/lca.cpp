#include "stdafx.h"

#include <queue>
#include <algorithm>

#include "lca.h"

std::string LCA::UP = "(#)";
std::string LCA::DOWN = "(@)";
std::string LCA::ROOT = "(*)";
std::string LCA::NONE = "(N)";

LCA::LCA() {
	for (int i = 0; i < (MAX_SENTENCE_SIZE << 1); ++i) {
		Clog2[i] = ceillog2(i);
		Flog2[i] = floorlog2(i);
	}
}

int LCA::ceillog2(int n) {
	--n;
	int r = 0;
	while (n > 0) {
		n >>= 1;
		++r;
	}
	return r;
}

int LCA::floorlog2(int n) {
	return ceillog2(n + 1) - 1;
}

void LCA::loadTree(std::vector<int> & heads) {

	int root = -1;
	for (int i = heads.size() - 1; i >= 0; --i) {
		if (heads[i] == -1) {
			root = i;
			break;
		}
	}

	size = heads.size();
	eular_size = (size << 1) - 1;
	int log_eular_size = ceillog2(eular_size);
	block_size = (log_eular_size >> 1) == 0 ? eular_size : log_eular_size >> 1;
	eular_size = eular_size % block_size == 0 ? eular_size : (eular_size / block_size + 1) * block_size;
	block_count = eular_size / block_size;
	int log_block_size = ceillog2(block_count);

	for (int i = 0; i < size; ++i) Index[i] = eular_size;
	std::vector<std::queue<int>> children;
	for (int i = 0; i < size; ++i) children.push_back(std::queue<int>());
	for (int i = 0; i < size; ++i) if (i != root) children[heads[i]].push(i);

	int seek = root;
	int pivot = 0;
	int depth = 0;
	while (!children[root].empty()) {
		Eular[pivot] = seek;
		Depth[pivot] = depth;
		Index[seek] = std::min(Index[seek], pivot);
		++pivot;
		if (children[seek].empty()) {
			seek = heads[seek];
			children[seek].pop();
			--depth;
		}
		else {
			++depth;
			seek = children[seek].front();
		}
	}

	while (++pivot < eular_size) {
		Depth[pivot] = Depth[pivot - 1] + 1;
	}

	for (int i = 0, n = 1 << block_size; i < n; ++i) {
		BeState[i] = false;
	}

	for (int i = 0, p = 0; i < block_count; ++i) {
		int n = p + block_size, s = 1, min = Depth[p], idx = p;

		while (++p < n) {

			s <<= 1;
			if (Depth[p] > Depth[p - 1]) {
				++s;
			}
			else if (min > Depth[p]) {
				min = Depth[p];
				idx = p;
			}
		}

		GroupMin[i] = min;
		GroupIndex[i] = idx;
		GroupState[i] = s;

		p -= block_size;

		if (BeState[s] == false) {
			BeState[s] = true;
			for (int j = 0; j < block_size; ++j) {
				InGroup[s][j][j] = j;
			}
			for (int l = 1; l < block_size; ++l) {
				for (int j = 0, m = block_size - l; j < m; ++j) {
					int k = InGroup[s][j][j + l - 1];
					InGroup[s][j][j + l] = Depth[j + l + p] < Depth[k + p] ? j + l : k;
				}
			}
		}

		p += block_size;
	}

	for (int i = 0; i < block_count; ++i) {
		BetweenGroup[i][0] = i;

	}

	for (int l = 1; l < log_block_size; ++l) {
		for (int i = 0, n = block_count - (1 << l); i <= n; ++i) {
			int j = BetweenGroup[i][l - 1];
			int k = BetweenGroup[i + (1 << (l - 1))][l - 1];
			BetweenGroup[i][l] = GroupMin[j] < GroupMin[k] ? j : k;
		}
	}
}

int LCA::query(int i, int j) {
	int l = Index[i], r = Index[j];
	if (l > r) {
		int temp = l;
		l = r;
		r = temp;
	}
	int sl = l / block_size, sr = r / block_size;
	if (sl == sr) {
		int base = sl * block_size;
		return Eular[InGroup[GroupState[sl]][l - base][r - base] + base];
	}
	else {
		int basel = sl * block_size;
		int baser = sr * block_size;
		int ml = InGroup[GroupState[sl]][l - basel][block_size - 1] + basel;
		int mr = InGroup[GroupState[sr]][0][r - baser] + baser;
		int mi;
		if (Depth[ml] < Depth[mr]) {
			mi = ml;
		}
		else {
			mi = mr;
		}
		if (sr - sl == 1) {
			return Eular[mi];
		}
		else {
			int l2 = Flog2[sr - sl - 1];
			int mt = BetweenGroup[sl + 1][l2];
			if (GroupMin[mt] < Depth[mi]) {
				mi = GroupIndex[mt];
			}
			mt = BetweenGroup[sr - (1 << l2)][l2];
			if (GroupMin[mt] < Depth[mi]) {
				mi = GroupIndex[mt];
			}
			return Eular[mi];
		}
	}
}

void LCA::loadPath(DependencyTree & tree) {

	POSPath.clear();
	FPOSPath.clear();
	if (tree.size() <= 1) {
		return;
	}

	std::vector<int> heads;
	heads.push_back(-1);
	for (int i = 0, n = tree.size(); i < n; ++i) {
		pred[i] = -1;
	}
	for (int i = 0, n = tree.size(); i < n; ++i) {
		int idx = TREENODE_HEAD(tree[i]);
		heads.push_back(idx + 1);
		head[i] = idx < i ? idx : -1;
		if (idx >= 0 && i < idx) {
			pred[idx] = i;
		}
	}
	loadTree(heads);
	for (int i = 1, n = tree.size(); i <= n; ++i) {
		POSPath.push_back(std::vector<std::string>());
		FPOSPath.push_back(std::vector<std::string>());
		for (int j = 1; j <= n; ++j) {
			int si = i, sj = j;
			int r = query(si, sj);
			int left_dep = 0, right_dep = 0;
			std::string lp = "";
			std::string lfp = "";
			while (si != r) {
				lp += TREENODE_POSTAG(tree[si - 1]) + LCA::UP;
				lfp += TREENODE_POSTAG(tree[si - 1]).substr(0, 1) + LCA::UP;
				si = heads[si];
				++left_dep;
			}
			if (r == 0) {
				lp += NONE;
				lfp += NONE;
			}
			else {
				lp += TREENODE_POSTAG(tree[si - 1]) + LCA::UP;
				lfp += TREENODE_POSTAG(tree[si - 1]).substr(0, 1) + LCA::UP;
			}
			std::string rp = "";
			std::string rfp = "";
			while (sj != r) {
				rp = TREENODE_POSTAG(tree[sj - 1]) + LCA::DOWN + rp;
				rfp = TREENODE_POSTAG(tree[sj - 1]).substr(0, 1) + LCA::DOWN + rfp;
				sj = heads[sj];
				++right_dep;
			}
			POSPath[i - 1].push_back(std::min(left_dep, right_dep) <= 2 ? lp + rp : LCA::NONE);
			FPOSPath[i - 1].push_back(std::min(left_dep, right_dep) <= 2 ? lfp + rfp : LCA::NONE);
		}
	}
}
