#ifndef _POSTAG_WEIGHT_H
#define _POSTAG_WEIGHT_H

#include "postag_macros.h"
#include "common\token\word.h"
#include "common\token\pos.h"
#include "common/parser/weight_base.h"
#include "include/learning/perceptron/packed_score.h"

namespace POSTagging {
	class Weight {
	protected:
		bool m_bTrain;
		std::string m_sReadPath;
		std::string m_sRecordPath;
		std::string m_sDictPath;

	public:

		TWord map_words;
		TPOSTag map_postags;
	public:

		WordTagIntMap map_word_tag_int;
		WordTagIntMap map_preword_tag_int;
		WordTagIntMap map_nextword_tag_int;
		WordTagIntMap map_pppword_tag_int;
		WordTagIntMap map_nnnword_tag_int;
		WordTagIntMap map_prepreword_tag_int;
		WordTagIntMap map_nextnextword_tag_int;

		CharWordTagIntMap map_tail_word_tag_int;
		CharWordTagIntMap map_head_word_tag_int;
		CharTagIntMap map_tail_tag_int;
		CharTagIntMap map_head_tag_int;
		CharTagIntMap map_char_tag_int;

		CharCharTagIntMap map_head2_tag_int;
		CharCharTagIntMap map_tail2_tag_int;

		CharCharCharTagIntMap map_head3_tag_int;
		CharCharCharTagIntMap map_tail3_tag_int;

		CharTagIntMap map_prehead_tag_int;

		WordWordWordTagIntMap map_pre_pre_now_tag_int;
		WordWordWordTagIntMap map_pre_now_next_tag_int;
		WordWordWordTagIntMap map_now_next_next_tag_int;

		WordWordTagIntMap map_pre_pre_tag_int;
		WordWordTagIntMap map_pre_now_tag_int;
		WordWordTagIntMap map_now_next_tag_int;
		WordWordTagIntMap map_next_next_tag_int;
		WordWordTagIntMap map_ppp_pp_tag_int;
		WordWordTagIntMap map_nn_nnn_tag_int;

		WordTagTagIntMap map_pre_pretag_tag_int;
		WordTagWordTagIntMap map_pre_pretag_now_tag_int;
		TagTagIntMap map_tag_tag_int;
		TagTagTagIntMap map_tag_tag_tag_int;

		CharCharCharTagIntMap map_tail_word_head_tag_int;


	public:
		Weight(const std::string & sRead, const std::string & sRecord, bool bTrain);
		~Weight();

		void loadScores();
		void saveScores() const;
		void debug();
		void computeAverageFeatureWeights(const int & round);
	};
}

#endif
