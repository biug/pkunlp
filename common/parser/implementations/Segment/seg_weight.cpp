#include "stdafx.h"
#include <fstream>

#include "seg_weight.h"
#include "common/oldtoken/word.h"
#include "common/oldtoken/pos.h"
#include "include/ngram.h"

namespace Segment {
	Weight::Weight(const std::string & sRead, const std::string & sRecord, const std::string & sDict, bool bTrain) :
		
		m_bTrain(bTrain), m_sReadPath(sRead), m_sRecordPath(sRecord), m_sDictPath(sDict),

		map_pos_char_tag_int("map_pos_char_tag_int"),
		map_pos_char_char_tag_int("map_pos_char_char_tag_int"),
		map_pos_prechar_nextchar_tag_int("map_pos_prechar_nextchar_tag_int"),

		map_pos_idc_tag_int1("map_pos_idc_tag_int1"),
		map_pos_idc_tag_int2("map_pos_idc_tag_int2"),

	    map_pos_idw_tag_int1("map_pos_idw_tag_int1"),
		map_pos_idw_tag_int2("map_pos_idw_tag_int2"),

		map_pos_pos_idw_tag_int1("map_pos_pos_idw_tag_int1"),
		map_pos_pos_idw_tag_int2("map_pos_pos_idw_tag_int2"),
		map_pos_pos_dic_tag_int("map_pos_pos_dic_tag_int"),

		map_tag_tag_int("map_tag_tag_int")

	{
		loadDicScores();
		loadScores();
	}

	Weight::~Weight() = default;


	void Weight::loadDicScores()
	{
		map_dic_int.clear();

		std::ifstream input(m_sDictPath);
		std::string word, type;
		map_dic_int.clear();
		while (input >> word >> type)
		{
			if (!word.empty())
				map_dic_int[word].push_back("dic="+type),map_words.code("dic="+type);
		}
	}

	void Weight::loadScores() {

		if (m_sReadPath.empty()) {
			return;
		}
		std::ifstream input(m_sReadPath);
		if (!input) {
			return;
		}

		input >> map_words.getTokenizer();
		input >> map_postags.getTokenizer();

		input >> map_pos_char_tag_int;
		input >> map_pos_char_char_tag_int;
		input >> map_pos_prechar_nextchar_tag_int;

		input >> map_pos_idc_tag_int1;
		input >> map_pos_idc_tag_int2;

		input >> map_pos_idw_tag_int1;
		input >> map_pos_idw_tag_int2;

		input >> map_pos_pos_idw_tag_int1;
		input >> map_pos_pos_idw_tag_int2;
		input >> map_pos_pos_dic_tag_int;
		
		input >> map_tag_tag_int;

		input.close();
	}

	void Weight::saveScores() const {

		if (m_sRecordPath.empty()) {
			return;
		}
		std::ofstream output(m_sRecordPath);
		if (!output) {
			return;
		}

		output << map_words.getTokenizer();
		output << map_postags.getTokenizer();

		output << map_pos_char_tag_int;
		output << map_pos_char_char_tag_int;
		output << map_pos_prechar_nextchar_tag_int;

		output << map_pos_idc_tag_int1;
		output << map_pos_idc_tag_int2;

		output << map_pos_idw_tag_int1;
		output << map_pos_idw_tag_int2;

		output << map_pos_pos_idw_tag_int1;
		output << map_pos_pos_idw_tag_int2;
		output << map_pos_pos_dic_tag_int;

		output << map_tag_tag_int;

		output.close();
	}
	void Weight::debug()
	{

		std::cout << map_words.getTokenizer();
		std::cout << map_postags.getTokenizer();

		std::cout << map_pos_char_tag_int;
		std::cout << map_pos_char_char_tag_int;

		std::cout << map_pos_idc_tag_int1;
		std::cout << map_pos_idc_tag_int2;

		std::cout << map_pos_idw_tag_int1;
		std::cout << map_pos_idw_tag_int2;

		std::cout << map_pos_pos_idw_tag_int1;
		std::cout << map_pos_pos_idw_tag_int2;

		std::cout << map_tag_tag_int;

	}
	void Weight::computeAverageFeatureWeights(const int & round) {

		map_pos_char_tag_int.computeAverage(round);
		map_pos_char_char_tag_int.computeAverage(round);
		map_pos_prechar_nextchar_tag_int.computeAverage(round);;

		map_pos_idc_tag_int1.computeAverage(round);
		map_pos_idc_tag_int2.computeAverage(round);

		map_pos_idw_tag_int1.computeAverage(round);
		map_pos_idw_tag_int2.computeAverage(round);

		map_pos_pos_idw_tag_int1.computeAverage(round);
		map_pos_pos_idw_tag_int2.computeAverage(round);
		map_pos_pos_dic_tag_int.computeAverage(round);

		map_tag_tag_int.computeAverage(round);

	}
}
