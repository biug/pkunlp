#include "LibGrass.h"
#pragma comment(lib, "..\\x64\\Release\\LibGrass.lib")

// 输入文件编码需要UTF-8
int main()
{
	// 创建分词器
	create_segmentor("..\\feature\\segment.feat", "..\\feature\\segment.dic");
	// 对文件分词（输出文件中每个词以空格隔开）
	seg_file("..\\test.in", "..\\test.seg");
	// 释放分词器
	delete_segmentor();

	// 创建词性标注tagger
	create_postagger("..\\feature\\postag.feat");
	// 对分词后的文件词性标注
	tag_file("..\\test.seg", "..\\test.pos");
	// 释放tagger
	delete_postagger();

	// 将文件按句子分行
	sentence_per_line("..\\test.pos", "..\\test.sents");

	// 创建句法分析器
	create_syntax_parser("..\\feature\\syntax.feat");
	// 对句子进行句法分析
	// 输出文件是句法树，共四列
	// 第一、二列是词和词性
	// 第三列是词在句法树中的父亲的位置索引，-1表示根节点
	// 第四列是词指向父节点的边的label
	syntax_parse_file("..\\test.sents", "..\\test.syn");
	// 释放句法分析器
	delete_syntax_parser();

	// 创建语义分析器
	// 语义分析的格式符合conll08
	create_semantic_parser("..\\feature\\semantic.feat", "..\\feature\\pseudotree.feat");
	// 对句子进行语义分析
	// 输出文件是语意图
	semantic_parse_file("..\\test.sents", "..\\test.sem");
	// 释放语义分析器
	delete_semantic_parser();
}