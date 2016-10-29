#include "LibGrass.h"
#pragma comment(lib, "..\\x64\\Release\\LibGrass.lib")

// �����ļ�������ҪUTF-8
int main()
{
	// �����ִ���
	create_segmentor("..\\feature\\segment.feat", "..\\feature\\segment.dic");
	// ���ļ��ִʣ�����ļ���ÿ�����Կո������
	seg_file("..\\test.in", "..\\test.seg");
	// �ͷŷִ���
	delete_segmentor();

	// �������Ա�עtagger
	create_postagger("..\\feature\\postag.feat");
	// �Էִʺ���ļ����Ա�ע
	tag_file("..\\test.seg", "..\\test.pos");
	// �ͷ�tagger
	delete_postagger();

	// ���ļ������ӷ���
	sentence_per_line("..\\test.pos", "..\\test.sents");

	// �����䷨������
	create_syntax_parser("..\\feature\\syntax.feat");
	// �Ծ��ӽ��о䷨����
	// ����ļ��Ǿ䷨����������
	// ��һ�������Ǵʺʹ���
	// �������Ǵ��ھ䷨���еĸ��׵�λ��������-1��ʾ���ڵ�
	// �������Ǵ�ָ�򸸽ڵ�ıߵ�label
	syntax_parse_file("..\\test.sents", "..\\test.syn");
	// �ͷž䷨������
	delete_syntax_parser();

	// �������������
	// ��������ĸ�ʽ����conll08
	create_semantic_parser("..\\feature\\semantic.feat", "..\\feature\\pseudotree.feat");
	// �Ծ��ӽ����������
	// ����ļ�������ͼ
	semantic_parse_file("..\\test.sents", "..\\test.sem");
	// �ͷ����������
	delete_semantic_parser();
}