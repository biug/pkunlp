#include "stdafx.h"
#include <fstream>

#include "postag_weight.h"
#include "common/token/word.h"
#include "common/token/pos.h"
#include "include/ngram.h"

namespace POSTagging {
	Weight::Weight(const std::string & sRead, const std::string & sRecord, bool bTrain) :
		m_bTrain(bTrain), m_sReadPath(sRead), m_sRecordPath(sRecord),
		map_word_tag_int("map_word_tag_int"),
		map_preword_tag_int("map_preword_tag_int"),
		map_nextword_tag_int("map_nextword_tag_int"),
		map_prepreword_tag_int("map_prepreword_tag_int"),
		map_nextnextword_tag_int("map_nextnextword_tag_int"),
		map_pppword_tag_int("map_pppword_tag_int"),
		map_nnnword_tag_int("map_nnnword_tag_int"),

		map_tag_tag_int("map_tag_tag_int"),
		map_tag_tag_tag_int("map_tag_tag_tag_int"),

		map_pre_pre_tag_int("map_pre_pre_tag_int"),
		map_pre_now_tag_int("map_pre_now_tag_int"),
		map_now_next_tag_int("map_now_next_tag_int"),
		map_next_next_tag_int("map_pre_pre_tag_int"),
		map_ppp_pp_tag_int("map_ppp_pp_tag_int"),
		map_nn_nnn_tag_int("map_nn_nnn_tag_int"),
		map_tail_word_tag_int("map_tail_word_tag_int"),
		map_head_word_tag_int("map_head_word_tag_int"),
		map_tail_tag_int("map_tail_tag_int"),
		map_head_tag_int("map_head_tag_int"),

		map_pre_pre_now_tag_int("map_pre_pre_now_tag_int"),
		map_pre_now_next_tag_int("map_pre_now_next_tag_int"),
		map_now_next_next_tag_int("map_now_next_next_tag_int"),
		map_pre_pretag_now_tag_int("map_pre_pretag_now_tag_int"),
		map_tail_word_head_tag_int("map_tail_word_head_tag_int"),
		map_char_tag_int("map_char_tag_int"),

		map_head2_tag_int("map_head2_tag_int"),
		map_tail2_tag_int("map_tail2_tag_int"),
		map_head3_tag_int("map_head3_tag_int"),
		map_tail3_tag_int("map_tail3_tag_int")

	{
		loadScores();
	}

	Weight::~Weight() = default;

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
		input >> map_word_tag_int;
		input >> map_preword_tag_int;
		input >> map_nextword_tag_int; 
		input >> map_prepreword_tag_int;
		input >> map_nextnextword_tag_int;
		input >> map_pppword_tag_int;
		input >> map_nnnword_tag_int;

		input >> map_tag_tag_int;
		input >> map_tag_tag_tag_int;
		
		input >> map_pre_pre_tag_int; 
		input >> map_pre_now_tag_int;
		input >> map_now_next_tag_int;
		input >> map_next_next_tag_int;
		input >> map_ppp_pp_tag_int;
		input >> map_nn_nnn_tag_int;

		input >> map_tail_word_tag_int;
		input >> map_head_word_tag_int;
		input >> map_tail_tag_int;
		input >> map_head_tag_int;

		input >> map_pre_pre_now_tag_int;
		input >> map_pre_now_next_tag_int;
		input >> map_now_next_next_tag_int;
		input >> map_pre_pretag_now_tag_int;
		input >> map_tail_word_head_tag_int;
		input >> map_char_tag_int;

		input >> map_head2_tag_int;
		input >> map_tail2_tag_int;
		input >> map_head3_tag_int;
		input >> map_tail3_tag_int;

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

		output << map_word_tag_int;
		output << map_preword_tag_int;
		output << map_nextword_tag_int;
		output << map_prepreword_tag_int;
		output << map_nextnextword_tag_int;
		output << map_pppword_tag_int;
		output << map_nnnword_tag_int;

		output << map_tag_tag_int;
		output << map_tag_tag_tag_int;

		output << map_pre_pre_tag_int;
		output << map_pre_now_tag_int;
		output << map_now_next_tag_int;
		output << map_next_next_tag_int;
		output << map_ppp_pp_tag_int;
		output << map_nn_nnn_tag_int;

		output << map_tail_word_tag_int;
		output << map_head_word_tag_int;
		output << map_tail_tag_int;
		output << map_head_tag_int;

		output << map_pre_pre_now_tag_int;
		output << map_pre_now_next_tag_int;
		output << map_now_next_next_tag_int;
		output << map_pre_pretag_now_tag_int;

		output << map_tail_word_head_tag_int;

		output << map_char_tag_int;

		output << map_head2_tag_int;
		output << map_tail2_tag_int;
		output << map_head3_tag_int;
		output << map_tail3_tag_int;


		output.close();
	}
	void Weight::debug()
	{

		std::cout << map_words.getTokenizer() <<std::endl;

		std::cout << map_words.getTokenizer() << std::endl;

		std::cout << map_word_tag_int<<std::endl;
		std::cout << map_preword_tag_int<<std::endl;
		std::cout << map_nextword_tag_int<<std::endl;
		std::cout << map_tag_tag_int<<std::endl;
		std::cout << map_pre_pre_tag_int<<std::endl;
		std::cout << map_pre_now_tag_int<<std::endl;
		std::cout << map_now_next_tag_int<<std::endl;
		std::cout << map_next_next_tag_int<<std::endl;

	}
	void Weight::computeAverageFeatureWeights(const int & round) {
		map_word_tag_int.computeAverage(round);
		map_preword_tag_int.computeAverage(round);
		map_nextword_tag_int.computeAverage(round);
		map_prepreword_tag_int.computeAverage(round);
		map_nextnextword_tag_int.computeAverage(round);
		map_pppword_tag_int.computeAverage(round);
		map_nnnword_tag_int.computeAverage(round);

		map_tag_tag_int.computeAverage(round);
		map_tag_tag_tag_int.computeAverage(round);

		map_pre_pre_tag_int.computeAverage(round);
		map_pre_now_tag_int.computeAverage(round);
		map_now_next_tag_int.computeAverage(round);
		map_next_next_tag_int.computeAverage(round);
		map_ppp_pp_tag_int.computeAverage(round);
		map_nn_nnn_tag_int.computeAverage(round);

		map_tail_word_tag_int.computeAverage(round);
		map_head_word_tag_int.computeAverage(round);
		map_tail_tag_int.computeAverage(round);
		map_head_tag_int.computeAverage(round);

		map_pre_pre_now_tag_int.computeAverage(round);
		map_pre_now_next_tag_int.computeAverage(round);
		map_now_next_next_tag_int.computeAverage(round);
		map_pre_pretag_now_tag_int.computeAverage(round);
		map_tail_word_head_tag_int.computeAverage(round);
		map_char_tag_int.computeAverage(round);

		map_head2_tag_int.computeAverage(round);
		map_tail2_tag_int.computeAverage(round);
		map_head3_tag_int.computeAverage(round);
		map_tail3_tag_int.computeAverage(round);

	}
}
