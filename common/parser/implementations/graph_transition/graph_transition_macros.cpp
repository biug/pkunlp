#include "stdafx.h"

#include <map>
#include <set>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <unordered_map>
#include <unordered_set>

#include "graph_transition_macros.h"

std::istream & operator>>(std::istream & input, SuperTagCandidates & stc) {
	ttoken line;
	int size, key, candidate;
	input >> size;
	std::getline(input, line);
	while (size--) {
		std::getline(input, line);
		std::istringstream iss(line);
		iss >> key;
		while (iss >> candidate) {
			stc[key].push_back(candidate);
		}
	}
	return input;
}

std::ostream & operator<<(std::ostream & output, const SuperTagCandidates & stc) {
	output << stc.size() << std::endl;
	for (const auto & item : stc) {
		output << item.first;
		for (const auto & candi : item.second) {
			output << " " << candi;
		}
		output << std::endl;
	}
	return output;
}
