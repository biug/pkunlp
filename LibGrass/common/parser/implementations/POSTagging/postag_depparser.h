#ifndef _POSTAG_DEPPARSER_H
#define _POSTAG_DEPPARSER_H

#include <vector>
#include <string>
#include "common/parser/depparser_base.h"
#include "postag_weight.h"

namespace POSTagging {
	class DepParser {
	protected:
		Weight* m_Weight;
		bool m_bTrain;
	private:

		struct MyString
		{
			std::vector<int> mystring;
			int code;
		};

		static POSTag empty_tag;
		static POSTag start_tag;
		static POSTag end_tag;
		static Word empty_word;
		static Word start_word;
		static Word end_word;

		#define BEAM_SIZE 4

		//StateItem m_lItems[MAX_SENTENCE_SIZE][MAX_SENTENCE_SIZE];
		MyString wordsentence[MAX_SEG_SIZE];
		POSTag correct_tagintsentence[MAX_SEG_SIZE];
		POSTag tagintsentence[MAX_SEG_SIZE];

		//std::vector<Arc> correctArcs;
		TagSentence correctTagSentence;
		int sentencelength;
		int maxlen;
		int maxtag;

		//int MaxScore[MAX_SENTENCE_SIZE][MAX_TAG_SIZE];//��i��tag�����scosr
		//POSTag PreTag[MAX_SENTENCE_SIZE][MAX_TAG_SIZE];//��i��tag��ǰһ��tag

		PackedScoreType<EISNER_PACKED_SIZE> packed_scores;

		int m_nTotalErrors;
		int m_nScoreIndex;
		int m_nTrainingRound;
		int totaltag;
		int totaltagerr;
		int testround;

		int prewordint, nowwordint, nextwordint, preprewordint, nextnextwordint;
		int pppwordint, nnnwordint;
		std::string preword, nowword, nextword;

		WordTagInt word_tag_int;
		TagTagInt tag_tag_int;

		TagTagTagInt tag_tag_tag_int;
		WordTagInt preword_tag_int;
		WordTagInt nextword_tag_int;
		WordTagInt prepreword_tag_int;
		WordTagInt nextnextword_tag_int;
		WordTagInt pppword_tag_int;
		WordTagInt nnnword_tag_int;

		CharWordTagInt tail_word_tag_int;
		CharWordTagInt head_word_tag_int;

		CharCharCharTagInt tail_word_head_tag_int;

		CharTagInt head_tag_int;
		CharTagInt tail_tag_int;
		CharTagInt char_tag_int;

		CharCharTagInt head_char_tag_int;
		CharCharTagInt tail_char_tag_int;

		WordWordTagInt pre_pre_tag_int;
		WordWordTagInt pre_now_tag_int;
		WordWordTagInt now_next_tag_int;
		WordWordTagInt next_next_tag_int;
		WordWordTagInt ppp_pp_tag_int;
		WordWordTagInt nn_nnn_tag_int;

		WordWordWordTagInt pre_pre_now_tag_int;
		WordWordWordTagInt pre_now_next_tag_int;
		WordWordWordTagInt now_next_next_tag_int;

		WordTagTagInt pre_pretag_tag_int;
		WordTagWordTagInt pre_pretag_now_tag_int;
		
		CharCharTagInt head2_tag_int;
		CharCharTagInt tail2_tag_int;

		CharCharCharTagInt head3_tag_int;
		CharCharCharTagInt tail3_tag_int;

	public:
		DepParser(const std::string & sFeatureInput, const std::string & sFeatureOut, bool bTrain);
		~DepParser();

		tscore get_score(int position, POSTag pre_pre_tag, POSTag pre_tag, POSTag now_tag , bool test  );
		void train(const WordSentence & wordsentence, const TagSentence & correct, int round);
		void finishtraining();
		void finishparsing();
		void parse(const WordSentence & sentence, const TagSentence & correct, TagSentence * retval);
		void getOrUpdateStackScore(int position, POSTag pre_pre_tag, POSTag pre_tag, POSTag now_tag ,int amount = 0);
		void work(TagSentence * retval, const TagSentence & correct, bool bTrain,bool test);
		void toMyString_ANSI(std::string s,int pos);
		void toMyString_UTF8(std::string s, int pos);
	};
}

#endif
