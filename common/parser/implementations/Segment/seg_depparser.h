#ifndef _Segment_DEPPARSER_H
#define _Segment_DEPPARSER_H

#include <vector>
#include <string>
#include <set>
#include "common/parser/depparser_base.h"
#include "seg_weight.h"

namespace Segment {
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
#define MAX_CHARSENTENCE_SIZE 10000
#define MAX_SEGTAG_SIZE 6

		std::vector<MyString> wordsentence;
		POSTag correct_tagintsentence[MAX_CHARSENTENCE_SIZE];
		POSTag tagintsentence[MAX_CHARSENTENCE_SIZE];

		Char charsentence[MAX_CHARSENTENCE_SIZE];
		POSTag correctTagSentence[MAX_CHARSENTENCE_SIZE];	

		int sentencelength;
		int maxlen;
		int maxtag;

		int MaxScore[MAX_CHARSENTENCE_SIZE][MAX_SEGTAG_SIZE];
		POSTag PreTag[MAX_CHARSENTENCE_SIZE][MAX_SEGTAG_SIZE];

		PackedScoreType<EISNER_PACKED_SIZE> packed_scores;

		int m_nTotalErrors;
		int m_nScoreIndex;
		int m_nTrainingRound;
		int totaltag;
		int totaltagerr;
		int totaltagcor;
		int testround;
		std::set<Char> num_set,pun_set;

		int prewordint, nowwordint, nextwordint, preprewordint, nextnextwordint;
		std::string preword, nowword, nextword;

		PosWordTagInt pos_char_tag_int;

		PosWordWordTagInt pos_char_char_tag_int;

		PosIdenTagInt pos_idc_tag_int1;
		PosIdenTagInt pos_idc_tag_int2;

		PosIdenTagInt pos_idw_tag_int1;
		PosIdenTagInt pos_idw_tag_int2;

		PosPosIdenTagInt pos_pos_idw_tag_int1;
		PosPosIdenTagInt pos_pos_idw_tag_int2;
		PosPosIdenTagInt pos_pos_dic_tag_int;
		TagTagInt tag_tag_int;

		void segsentence(std::vector<std::vector<Char> > & sent_list);
		void toMyString_ANSI(std::string s);
		void toMyString_UTF8(std::string s);
		void prework();

	public:
		DepParser(const std::string & sFeatureInput, const std::string & sFeatureOut, const std::string & sDictPath, bool bTrain);
		~DepParser();

		tscore get_score(int position, POSTag pre_tag, POSTag now_tag , bool test  );
		void train(const WordSentence & wordsentence, int round);
		void finishtraining();
		void finishparsing();
		void parse(const std::string & sentence, TagSentence * retval);
		void getOrUpdateStackScore(int position, POSTag pre_tag, POSTag now_tag ,int amount = 0);
		void work(TagSentence * retval, bool bTrain, bool test);		
	};
}

#endif
