#ifndef _MACROS_H
#define _MACROS_H

#include "stdafx.h"

#include <tuple>
#include <vector>

#include "include/ngram.h"
#include "common/oldtoken/token.h"

#define MAX_SENTENCE_SIZE 256
#define MAX_SENTENCE_BITS 8

#define ROOT_WORD		"-ROOT-"
#define ROOT_POSTAG		"-ROOT-"
#define ROOT_DEPLABEL	"-ROOT-"
#define EMPTY_WORD		"-EMPTY-"
#define EMPTY_POSTAG	"-EMPTY-"
#define START_WORD		"-START-"
#define START_POSTAG	"-START-"
#define MIDDLE_WORD		"-MIDDLE-"
#define MIDDLE_POSTAG	"-MIDDLE-"
#define END_WORD		"-END-"
#define END_POSTAG		"-END-"

#define NULL_LABEL		"-NULL-"

#define IS_NULL(X)				((X) == -1)
#define IS_EMPTY(X)				((X) >= MAX_SENTENCE_SIZE)

#define SENT_SPTOKEN	"/"
#define SENT_WORD(X)	(std::get<0>(X))
#define SENT_POSTAG(X)	(std::get<1>(X))

#define TREENODE_WORD(X)			(std::get<0>(std::get<0>(X)))
#define TREENODE_POSTAG(X)			(std::get<1>(std::get<0>(X)))
#define TREENODE_POSTAGGEDWORD(X)	(std::get<0>(X))
#define TREENODE_HEAD(X)			(std::get<1>(X))
#define TREENODE_LABEL(X)			(std::get<2>(X))

#define GRAPH_LEFT	-1
#define GRAPH_RIGHT 1

typedef int gtype;

typedef unsigned int Unsigned;
typedef int Int;
typedef gtype Word;
typedef gtype POSTag;
typedef gtype SuperTag;

typedef TokenOld DWord;
typedef TokenOld DPOSTag;
typedef TokenOld DSuperTag;
typedef TokenOld DLabel;

typedef BiGram<gtype> TwoInts;
typedef BiGram<gtype> TwoWords;
typedef BiGram<gtype> POSTagSet2;
typedef BiGram<gtype> WordPOSTag;
typedef QuarGram<unsigned int> WordSetOfDepLabels;
typedef QuarGram<unsigned int> POSTagSetOfDepLabels;

typedef TriGram<gtype> ThreeInts;
typedef TriGram<gtype> ThreeWords;
typedef TriGram<gtype> POSTagSet3;
typedef TriGram<gtype> WordIntInt;
typedef TriGram<gtype> POSTagIntInt;
typedef TriGram<gtype> WordWordPOSTag;
typedef TriGram<gtype> WordPOSTagPOSTag;

typedef QuarGram<gtype> FourInts;
typedef QuarGram<gtype> POSTagSet4;
typedef QuarGram<gtype> ThreeWordsInt;
typedef QuarGram<gtype> WordWordPOSTagPOSTag;
typedef QuarGram<gtype> WordWordPOSTagInt;

typedef BiGram<gtype> WordInt;
typedef BiGram<gtype> POSTagInt;

typedef TriGram<gtype> TwoWordsInt;
typedef TriGram<gtype> POSTagSet2Int;
typedef TriGram<gtype> WordPOSTagInt;
typedef TriGram<Unsigned> ThreeUnsignedS;

typedef QuarGram<gtype> POSTagSet3Int;
typedef QuarGram<gtype> TwoWordsIntInt;
typedef QuarGram<gtype> POSTagSet2IntInt;
typedef QuarGram<gtype> WordWordPOSTagInt;
typedef QuarGram<gtype> WordPOSTagPOSTagInt;

typedef QuinGram<gtype> POSTagSet4Int;
typedef QuinGram<gtype> WordWordPOSTagPOSTagInt;
typedef QuinGram<gtype> WordPOSTagPOSTagPOSTagInt;

typedef HexaGram<gtype> POSTagSet5Int;
typedef HexaGram<gtype> WordPOSTagPOSTagPOSTagPOSTagInt;

typedef std::tuple<ttokenold, ttokenold> POSTaggedWord;
typedef std::tuple<POSTaggedWord, int, ttokenold> DependencyTreeNode;
typedef std::pair<DependencyTreeNode, std::vector<int>> DependencyPathTreeNode;
typedef std::pair<DependencyTreeNode, ttokenold> DependencyTaggedTreeNode;

typedef std::vector<POSTaggedWord> Sentence;
typedef std::vector<DependencyTreeNode> DependencyTree;
typedef std::vector<DependencyPathTreeNode> DependencyPathTree;
typedef std::vector<DependencyTaggedTreeNode> DependencyTaggedTree;

typedef std::unordered_map<int, std::vector<int>> SuperTagCandidates;

int encodeLinkDistance(const int & st, const int & n0);
int encodeLinkDistanceOrDirection(const int & hi, const int & di, bool dir);
std::string nCharPrev(const Sentence & sent, int index, int n);
std::string nCharNext(const Sentence & sent, int index, int n);

std::istream & operator>>(std::istream & input, Sentence & sentence);
std::istream & operator>>(std::istream & input, DependencyTree & tree);
std::istream & operator>>(std::istream & input, DependencyPathTree & tree);
std::istream & operator>>(std::istream & input, DependencyTaggedTree & tree);

std::ostream & operator<<(std::ostream & output, const Sentence & sentence);
std::ostream & operator<<(std::ostream & output, const DependencyTree & tree);
std::ostream & operator<<(std::ostream & output, const DependencyPathTree & tree);
std::ostream & operator<<(std::ostream & output, const DependencyTaggedTree & tree);

typedef std::vector<std::string> WordSentence;
typedef std::vector<std::string> TagSentence;
typedef std::vector<int> CharSentence;

struct WordTagged
{
	WordSentence wordsentence;
	TagSentence tagsentence;
};

std::string GBK2UTF8(const std::string & sGBK);
std::string UTF82GBK(const std::string & sUTF8);

void nBackSpace(const std::string & str);

#endif
