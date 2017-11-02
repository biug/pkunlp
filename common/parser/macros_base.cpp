#include "stdafx.h"
#include <map>
#include <set>
#include <stack>
#include <cstring>
#include <fstream>
#include <sstream>
#include <algorithm>

#include "macros_base.h"
#ifdef WIN32
	#include <windows.h>
#else
	#include "common/iconv-wrapper/iconv.hpp"
#endif


int encodeLinkDistance(const int & st, const int & n0) {
	int diff = n0 - st;
	if (diff > 10) {
		diff = 6;
	}
	else if (diff > 5) {
		diff = 5;
	}
	return diff;
}

int encodeLinkDistanceOrDirection(const int & hi, const int & di, bool dir) {
	int diff = hi - di;
	if (dir) {
		return diff > 0 ? 7 : -7;
	}
	if (diff < 0) {
		diff = -diff;
	}
	if (diff > 10) {
		diff = 6;
	}
	else if (diff > 5) {
		diff = 5;
	}
	if (hi < di) {
		diff = -diff;
	}
	return diff;
}

std::string nCharPrev(const Sentence & sent, int index, int n) {
	std::string str = "";
	--index;
	while (index >= 0 && n > 0) {
		const std::string & word = SENT_WORD(sent[index]);
		int i = word.length() - 1;
		while (i >= 0) {
			const unsigned char & c = word[i];
			if ((c >> 6) != 2) {
				--n;
				if (n == 0) {
					break;
				}
			}
			--i;
		}
		str = word.substr(i == -1 ? 0 : i) + str;
		--index;
	}
	while (n--) {
		str = "(P)" + str;
	}
	return str;
}

std::string nCharNext(const Sentence & sent, int index, int n) {
	std::string str = "";
	++index;
	while (index < sent.size() && n > 0) {
		const std::string & word = SENT_WORD(sent[index]);
		int i = 1;
		while (i < word.length()) {
			const unsigned char & c = word[i];
			if ((c >> 6) != 2) {
				--n;
				if (n == 0) {
					break;
				}
			}
			++i;
		}
		if (i == word.length()) {
			--n;
		}
		str = str + word.substr(0, i);
		++index;
	}
	while (n--) {
		str = str + "(N)";
	}
	return str;
}

std::istream & operator>>(std::istream & input, Sentence & sentence) {
	sentence.clear();
	ttokenold line, token;
	std::getline(input, line);
	std::istringstream iss(line);
	while (iss >> token) {
		int i = token.rfind(SENT_SPTOKEN);
		sentence.push_back(POSTaggedWord(token.substr(0, i), token.substr(i + 1)));
	}
	return input;
}

std::istream & operator>>(std::istream & input, DependencyTree & tree) {
	tree.clear();
	ttokenold line, token;
	while (true) {
		std::getline(input, line);
		if (line.empty()) {
			break;
		}
		DependencyTreeNode node;
		std::istringstream iss(line);
		iss >> TREENODE_WORD(node) >> TREENODE_POSTAG(node) >> TREENODE_HEAD(node) >> TREENODE_LABEL(node);
		tree.push_back(node);
	}
	return input;
}

std::istream & operator>>(std::istream & input, DependencyPathTree & tree) {
	tree.clear();
	ttokenold line, token;
	while (true) {
		std::getline(input, line);
		if (line.empty()) {
			break;
		}
		DependencyPathTreeNode node;
		std::istringstream iss(line);
		iss >> TREENODE_WORD(node.first) >> TREENODE_POSTAG(node.first) >> TREENODE_HEAD(node.first) >> TREENODE_LABEL(node.first);
		int head;
		while (iss >> head) {
			node.second.push_back(head);
		}
		tree.push_back(node);
	}
	return input;
}

std::istream & operator>>(std::istream & input, DependencyTaggedTree & tree) {
	tree.clear();
	ttokenold line, token;
	while (true) {
		std::getline(input, line);
		if (line.empty()) {
			break;
		}
		DependencyTaggedTreeNode node;
		std::istringstream iss(line);
		iss >> TREENODE_WORD(node.first) >> TREENODE_POSTAG(node.first) >> TREENODE_HEAD(node.first) >> TREENODE_LABEL(node.first) >> node.second;
		tree.push_back(node);
	}
	return input;
}

std::ostream & operator<<(std::ostream & output, const Sentence & sentence) {
	auto itr = sentence.begin();
	while (true) {
		output << SENT_WORD(*itr) << SENT_SPTOKEN << SENT_POSTAG(*itr);
		if (++itr == sentence.end()) {
			break;
		}
		else {
			output << " ";
		}
	}
	output << std::endl;
	return output;
}

std::ostream & operator<<(std::ostream & output, const DependencyTree & tree) {
	for (auto itr = tree.begin(); itr != tree.end(); ++itr) {
		output << TREENODE_WORD(*itr) << "\t" << TREENODE_POSTAG(*itr) << "\t" << TREENODE_HEAD(*itr) << "\t" << TREENODE_LABEL(*itr) << std::endl;
	}
	output << std::endl;
	return output;
}

std::ostream & operator<<(std::ostream & output, const DependencyPathTree & tree) {
	for (auto itr = tree.begin(); itr != tree.end(); ++itr) {
		output << TREENODE_WORD(itr->first) << "\t" << TREENODE_POSTAG(itr->first) << "\t" << TREENODE_HEAD(itr->first) << "\t" << TREENODE_LABEL(itr->first);
		for (const auto & head : itr->second) {
			output << "\t" << head;
		}
		output << std::endl;
	}
	output << std::endl;
	return output;
}

std::ostream & operator<<(std::ostream & output, const DependencyTaggedTree & tree) {
	for (auto itr = tree.begin(); itr != tree.end(); ++itr) {
		output << TREENODE_WORD(itr->first) << "\t" << TREENODE_POSTAG(itr->first) << "\t" << TREENODE_HEAD(itr->first) << "\t" << TREENODE_LABEL(itr->first) << "\t" << itr->second << std::endl;
	}
	output << std::endl;
	return output;
}

#ifdef _WIN32
std::string GBK2UTF8(const std::string & sGBK) {
	std::string sUTF8 = "";
	WCHAR * str1;
	int n = MultiByteToWideChar(CP_ACP, 0, sGBK.c_str(), -1, NULL, 0);
	str1 = new WCHAR[n];
	MultiByteToWideChar(CP_ACP, 0, sGBK.c_str(), -1, str1, n);
	n = WideCharToMultiByte(CP_UTF8, 0, str1, -1, NULL, 0, NULL, NULL);
	char * str2 = new char[n];
	WideCharToMultiByte(CP_UTF8, 0, str1, -1, str2, n, NULL, NULL);
	sUTF8 = str2;
	delete[]str1;
	delete[]str2;
	return sUTF8;
}

std::string UTF82GBK(const std::string & sUTF8) {
	std::string sGBK = "";
	WCHAR * str1;
	int n = MultiByteToWideChar(CP_UTF8, 0, sUTF8.c_str(), -1, NULL, 0);
	str1 = new WCHAR[n];
	MultiByteToWideChar(CP_UTF8, 0, sUTF8.c_str(), -1, str1, n);
	n = WideCharToMultiByte(CP_ACP, 0, str1, -1, NULL, 0, NULL, NULL);
	char * str2 = new char[n];
	WideCharToMultiByte(CP_ACP, 0, str1, -1, str2, n, NULL, NULL);
	sGBK = str2;
	delete[]str1;
	delete[]str2;
	return sGBK;
}
#else
std::string GBK2UTF8(const std::string & sGBK) {
	std::string ret;
	iconvpp::converter reconv("GBK", "UTF-8");
	reconv.convert(sGBK, ret);
	return ret;
}

std::string UTF82GBK(const std::string & sUTF8) {
	std::string ret;
	iconvpp::converter reconv("UTF-8", "GBK");
	reconv.convert(sUTF8, ret);
	return ret;
}
#endif

void nBackSpace(const std::string & str) {
	for (int i = 0; i < str.size(); ++i) {
		std::cout << '\b';
	}
	std::cout << std::flush;
}
