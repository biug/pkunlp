#include "stdafx.h"
#include <cmath>
#include <stack>
#include <string>
#include <map>
#include <vector>
#include <unordered_set>
#include <queue>
#include <algorithm>
#include <ctime>

#include "postag_depparser.h"
#include "common/oldtoken/word.h"
#include "common/oldtoken/pos.h"
#include "postag_macros.h"

#define INF 0x3fffffff

namespace POSTagging {
	POSTag DepParser::empty_tag= POSTag();
	POSTag DepParser::start_tag = POSTag();
	POSTag DepParser::end_tag = POSTag();
	Word DepParser::empty_word = Word();
	Word DepParser::start_word = Word();
	Word DepParser::end_word = Word();

	struct TT
	{
		int pos, pretag, nowtag;

		bool operator < (const TT tmp)const
		{
			return pos < tmp.pos ||
				pos == tmp.pos && pretag < tmp.pretag ||
				pos == tmp.pos && pretag == tmp.pretag && nowtag < tmp.nowtag;
		}
	};

	std::string stringwordsentence[MAX_SEG_SIZE];
	int ftimes = 0;
	std::map<TT, int> map_test;

	DepParser::DepParser(const std::string & sFeatureInput, const std::string & sFeatureOut, bool bTrain) :
		m_bTrain(bTrain) {

		maxlen = 0;
		maxtag = 0;
		testround = 0;
		sentencelength = 0;
		m_nTotalErrors = 0;
		m_nTrainingRound = 0;
		totaltag = 0;
		totaltagerr = 0;
		m_nScoreIndex = bTrain ? eNonAverage : eAverage;
		//std::cout << sFeatureInput << std::endl;
		m_Weight = new Weight(sFeatureInput, sFeatureOut, bTrain);
		ftimes = 0;
		/*for (int i = 0; i < MAX_SENTENCE_SIZE; ++i) {
			m_lItems[1][i].init(i, i);
		}*/

		DepParser::empty_tag =	(m_Weight->map_postags.code(EMPTY_POSTAG));
		DepParser::start_tag =	(m_Weight->map_postags.code(START_POSTAG));
		DepParser::end_tag =	(m_Weight->map_postags.code(END_POSTAG));
		DepParser::empty_word =	(m_Weight->map_words.code(EMPTY_WORD));
		DepParser::start_word =	(m_Weight->map_words.code(START_WORD));
		DepParser::end_word =	(m_Weight->map_words.code(END_WORD));

	}

	DepParser::~DepParser() {
		delete m_Weight;
	}

	tscore DepParser::get_score(int position, POSTag pre_pre_tag, POSTag pre_tag , POSTag cur_tag,bool test) {

		
		if (test)
		{
			//return 10;
			TT tmp;
			tmp.pos = position;
			tmp.pretag = pre_tag;
			tmp.nowtag = cur_tag;
			if (map_test[tmp] == 1)
				return 10;
			else return -10;
		}

		else
		{
			packed_scores.reset();
			getOrUpdateStackScore(position, pre_pre_tag, pre_tag , cur_tag);
			return packed_scores[0];
		}		
	}

	void DepParser::toMyString_ANSI(std::string s,int pos)
	{
		wordsentence[pos].mystring.clear();
		wordsentence[pos].code = m_Weight->map_words.code(s);

		int i, num = 0;
		int st = 0;

		for (i = 0; i < s.size(); i++)
		{
			if (s[i] > 0)
				wordsentence[pos].mystring.push_back(s[i]);
			else
			{
				if (st == 0)
					num = -s[i];
				else if (st == 1)
				{
					num = num * 256 - s[i];
					wordsentence[pos].mystring.push_back(num);					
				}
				st = st ^ 1;
			}
		}
	}

	int getpreone(int x)
	{
		int num = 0;
		//std::cout << ' '<<x<<' '<<  std::endl;
		for (int i = 7; i >= 0; i--)
		{
			if (x < (1 << i)) break;
			else num++, x -= 1 << i;
		}
		return num;
	}

	void DepParser::toMyString_UTF8(std::string s, int pos)
	{
		wordsentence[pos].mystring.clear();
		wordsentence[pos].code = m_Weight->map_words.getTokenizer().find(s);

		int i;
		unsigned int num = 0;
		int st = 0;

		for (i = 0; i < s.size(); i++)
		{
			unsigned char ch = s[i];
			
			if (getpreone(ch) != 1)
			{
				if (num != 0) wordsentence[pos].mystring.push_back(num);
				num = ch;
			}					
			else				
				num = num * 256 + ch;		
		}
		wordsentence[pos].mystring.push_back(num);
	}

	std::string toBanJiao(std::string input) {
		std::string temp;
		for (size_t i = 0; i < input.size(); i++) {
			if (((input[i] & 0xF0) ^ 0xE0) == 0) {
				int old_char = (input[i] & 0xF) << 12 | ((input[i + 1] & 0x3F) << 6 | (input[i + 2] & 0x3F));
				if (old_char == 0x3000) { // blank
					char new_char = 0x20;
					temp += new_char;
				}
				else if (old_char >= 0xFF01 && old_char <= 0xFF5E) { // full char
					char new_char = old_char - 0xFEE0;
					if (new_char == ',' || new_char == '?' || new_char == '!') {
						temp += input[i];
						temp += input[i + 1];
						temp += input[i + 2];
					}
					else temp += new_char;
				}
				else { // other 3 bytes char
					temp += input[i];
					temp += input[i + 1];
					temp += input[i + 2];
				}
				i = i + 2;
			}
			else {
				temp += input[i];
			}
		}
		return temp;
	}

	void DepParser::train(const WordSentence & ref_sent , const TagSentence & correct, int round) {//训锟斤拷锟矫碉拷锟斤拷Tag
		// initialize
		int idx = 0;
		int df = 0;
		int test = false;
		sentencelength = ref_sent.size();

		WordSentence half_sent;
		for (auto & sent : ref_sent)
			half_sent.push_back(toBanJiao(sent));
			
		toMyString_UTF8(START_WORD, 0);
		for (auto itr = half_sent.begin(); itr != half_sent.end(); ++itr) {
			toMyString_UTF8(*itr, ++idx);
		}
		toMyString_UTF8(END_WORD, ++idx);
		toMyString_UTF8(EMPTY_WORD, ++idx);
		toMyString_UTF8(EMPTY_WORD, ++idx);

		idx = 0;
		correct_tagintsentence[0] = start_tag;

		for (auto itr = correct.begin(); itr != correct.end(); ++itr)
		{
			correct_tagintsentence[++idx] = m_Weight->map_postags.code(*itr);
		}
		correct_tagintsentence[++idx] = end_tag;
		// train
		work(nullptr, correct, true,test);
		m_nTrainingRound++;

		totaltag += sentencelength;
	}

	void DepParser::finishparsing()
	{
		std::cout << testround << " all done" << std::endl;
		//std::cout << m_nTotalErrors << " / " << testround << ' ' << 1 - 1.0*m_nTotalErrors / testround << std::endl;
		//std::cout << totaltagerr << " / " << totaltag << ' ' << 1 - 1.0*totaltagerr / totaltag << std::endl;
	}

	void DepParser::finishtraining() {
		
		Weight * cweight = (Weight*)m_Weight;
		cweight->computeAverageFeatureWeights(m_nTrainingRound);
		cweight->saveScores();
		std::cout << m_nTrainingRound << " all done" << std::endl;
		std::cout << m_nTotalErrors << " / " << m_nTrainingRound <<' '<< 1 - 1.0*m_nTotalErrors / m_nTrainingRound << std::endl;
		std::cout << totaltagerr << " / " << totaltag << ' ' << 1 - 1.0*totaltagerr / totaltag << std::endl;

	}

	void DepParser::parse(const WordSentence & ref_sent, const TagSentence & correct, TagSentence * retval) {//锟窖撅拷锟斤拷parse为Tag.

		int idx = 0;
		int test = false;

		WordSentence half_sent;
		for (auto & sent : ref_sent)
			half_sent.push_back(toBanJiao(sent));

		m_nTrainingRound = 0;
		sentencelength = half_sent.size();
		testround++;

		toMyString_UTF8(START_WORD, 0);
		for (auto itr = half_sent.begin(); itr != half_sent.end(); ++itr) {
			toMyString_UTF8(*itr, ++idx);
			stringwordsentence[idx] = *itr;
		}
		toMyString_UTF8(END_WORD, ++idx);
		toMyString_UTF8(EMPTY_WORD, ++idx);
		toMyString_UTF8(EMPTY_WORD, ++idx);

		idx = 0;
		correct_tagintsentence[0] = start_tag;

		for (auto itr = correct.begin(); itr != correct.end(); ++itr)
		{
			correct_tagintsentence[++idx] = m_Weight->map_postags.code(*itr);
		}
		correct_tagintsentence[++idx] = end_tag;
		totaltag += sentencelength;

		work(retval, correctTagSentence , false, test);

		if (testround % 100 == 0)
			std::cout << testround << " done" << std::endl;
	}

	void DepParser::getOrUpdateStackScore(int position, POSTag pre_pre_tag, POSTag pre_tag, POSTag cur_tag , int amount) {//统锟斤拷feature vector

		ftimes++;
		Weight * cweight = (Weight*)m_Weight;

		prewordint = wordsentence[position - 1].code;
		nowwordint = wordsentence[position].code;
		nextwordint = wordsentence[position + 1].code;

		std::vector<int> noww = wordsentence[position].mystring;
		
		if (position == 1) preprewordint = start_word;
		else preprewordint = wordsentence[position - 2].code;

		if (position == sentencelength) nextnextwordint = end_word;
		else nextnextwordint = wordsentence[position + 2].code;

		if (position - 3 < 1) pppwordint = start_word;
		else pppwordint = wordsentence[position - 3].code;
		
		if (position + 3 > sentencelength) nnnwordint = end_word;
		else nnnwordint = wordsentence[position + 3].code;

		word_tag_int.refer(nowwordint,cur_tag, 0);
		cweight->map_word_tag_int.getOrUpdateScore(packed_scores, word_tag_int, 0, m_nScoreIndex, amount, m_nTrainingRound);

		tag_tag_int.refer(pre_tag, cur_tag, 0);
		cweight->map_tag_tag_int.getOrUpdateScore(packed_scores, tag_tag_int, 0, m_nScoreIndex, amount, m_nTrainingRound);
		
		//if (position > 1)
		tag_tag_tag_int.refer(pre_pre_tag,pre_tag, cur_tag, 0),
		cweight->map_tag_tag_tag_int.getOrUpdateScore(packed_scores, tag_tag_tag_int, 0, m_nScoreIndex, amount, m_nTrainingRound);
		
		//if (position > 1)
		preword_tag_int.refer(prewordint, cur_tag, 0),
		cweight->map_preword_tag_int.getOrUpdateScore(packed_scores, preword_tag_int, 0, m_nScoreIndex, amount, m_nTrainingRound);

		//if (position < sentencelength)
		nextword_tag_int.refer(nextwordint, cur_tag, 0),
		cweight->map_nextword_tag_int.getOrUpdateScore(packed_scores, nextword_tag_int, 0, m_nScoreIndex, amount, m_nTrainingRound);

		//if (position > 2)
		prepreword_tag_int.refer(preprewordint, cur_tag, 0),
		cweight->map_prepreword_tag_int.getOrUpdateScore(packed_scores, prepreword_tag_int, 0, m_nScoreIndex, amount, m_nTrainingRound);
		//if (position < sentencelength - 1)
		nextnextword_tag_int.refer(nextnextwordint, cur_tag, 0),
		cweight->map_nextnextword_tag_int.getOrUpdateScore(packed_scores, nextnextword_tag_int, 0, m_nScoreIndex, amount, m_nTrainingRound);

		if (MAX_WINDOW_SIZE >= 3)
		{
			pppword_tag_int.refer(pppwordint, cur_tag, 0),
				cweight->map_pppword_tag_int.getOrUpdateScore(packed_scores, pppword_tag_int, 0, m_nScoreIndex, amount, m_nTrainingRound);
			//if (position < sentencelength - 1)
			nnnword_tag_int.refer(nnnwordint, cur_tag, 0),
				cweight->map_nnnword_tag_int.getOrUpdateScore(packed_scores, nnnword_tag_int, 0, m_nScoreIndex, amount, m_nTrainingRound);
		}

		//bigram
		pre_pre_tag_int.refer(preprewordint, prewordint, cur_tag, 0),
		cweight->map_pre_pre_tag_int.getOrUpdateScore(packed_scores, pre_pre_tag_int, 0, m_nScoreIndex, amount, m_nTrainingRound);

		pre_now_tag_int.refer(prewordint, nowwordint, cur_tag, 0),
		cweight->map_pre_now_tag_int.getOrUpdateScore(packed_scores, pre_now_tag_int, 0, m_nScoreIndex, amount, m_nTrainingRound);

		now_next_tag_int.refer(nowwordint, nextwordint, cur_tag, 0);
		cweight->map_now_next_tag_int.getOrUpdateScore(packed_scores, now_next_tag_int, 0, m_nScoreIndex, amount, m_nTrainingRound);
		
		next_next_tag_int.refer(nextwordint, nextnextwordint, cur_tag, 0);
		cweight->map_next_next_tag_int.getOrUpdateScore(packed_scores, next_next_tag_int, 0, m_nScoreIndex, amount, m_nTrainingRound);
		
		if (MAX_WINDOW_SIZE >= 3)
		{
			ppp_pp_tag_int.refer(pppwordint,preprewordint, cur_tag, 0),
				cweight->map_ppp_pp_tag_int.getOrUpdateScore(packed_scores, ppp_pp_tag_int, 0, m_nScoreIndex, amount, m_nTrainingRound);
			//if (position < sentencelength - 1)
			nn_nnn_tag_int.refer(nextnextwordint,nnnwordint, cur_tag, 0),
				cweight->map_nn_nnn_tag_int.getOrUpdateScore(packed_scores, nn_nnn_tag_int, 0, m_nScoreIndex, amount, m_nTrainingRound);
		}
	
	//one char one tag	
		tail_tag_int.refer(noww.back() , cur_tag, 0);
		cweight->map_tail_tag_int.getOrUpdateScore(packed_scores, tail_tag_int, 0, m_nScoreIndex, amount, m_nTrainingRound);

		head_tag_int.refer(noww.front() , cur_tag, 0);
		cweight->map_head_tag_int.getOrUpdateScore(packed_scores, head_tag_int, 0, m_nScoreIndex, amount, m_nTrainingRound);

		/*if (noww.size() == 1)
		{
			tail_word_head_tag_int.refer(wordsentence[position - 1].mystring.back(), nowwordint, wordsentence[position + 1].mystring.front(), cur_tag, 0);
			cweight->map_tail_word_head_tag_int.getOrUpdateScore(packed_scores, tail_word_head_tag_int, 0, m_nScoreIndex, amount, m_nTrainingRound);
		}*/

	//head23,tail23
		int h[3], t[3],si = noww.size();
		for (int i = 0; i < 3; i++)
		{
			if (i < si)
				h[i] = noww[i], t[i] = noww[si - i - 1];
			else h[i] = 0, t[i] = 0;
		}
		if (h[1] > 0)
		head2_tag_int.refer(h[0], h[1], cur_tag, 0),
		cweight->map_head2_tag_int.getOrUpdateScore(packed_scores, head2_tag_int, 0, m_nScoreIndex, amount, m_nTrainingRound);
		if (h[2] > 0)
		head3_tag_int.refer(h[0], h[1], h[2], cur_tag, 0),
		cweight->map_head3_tag_int.getOrUpdateScore(packed_scores, head3_tag_int, 0, m_nScoreIndex, amount, m_nTrainingRound);

		if (t[1] > 0)
		tail2_tag_int.refer(t[0], t[1], cur_tag, 0),
		cweight->map_tail2_tag_int.getOrUpdateScore(packed_scores, tail2_tag_int, 0, m_nScoreIndex, amount, m_nTrainingRound);
		if (t[2] > 0)
		tail3_tag_int.refer(t[0], t[1], t[2], cur_tag, 0),
		cweight->map_tail3_tag_int.getOrUpdateScore(packed_scores, tail3_tag_int, 0, m_nScoreIndex, amount, m_nTrainingRound);
	
	}


	struct TTS
	{
		POSTag pre_pre_tag, pre_tag,cur_tag;
		long long score;

		bool operator < (const TTS k)const
		{
			return score > k.score;
		}
	};
	std::vector<TTS>  MaxScore[MAX_SEG_SIZE];

	std::map<std::pair<POSTag, POSTag>,std::pair<POSTag,POSTag> > PreTag[MAX_SEG_SIZE];


	void DepParser::work(TagSentence * retval, const TagSentence & correct, bool bTrain,bool test) {//锟睫革拷锟斤拷锟斤拷

		int pos;
		POSTag pre_tag, cur_tag;
		int tag_size = m_Weight->map_postags.getTokenizer().count();

		tagintsentence[0] = start_tag;
		tagintsentence[sentencelength + 1] = end_tag;

		if (test)
		{
			map_test.clear();
			for (pos = 1; pos <= sentencelength + 1; pos++)
			{
				TT tmp;
				tmp.pos = pos;
				tmp.pretag = correct_tagintsentence[pos - 1];
				tmp.nowtag = correct_tagintsentence[pos];
				map_test[tmp] = 1;
			}
		}
		//memset(MaxScore, 0, sizeof(MaxScore));memset(PreTag, 0, sizeof(PreTag));

		TTS start;
		start.pre_pre_tag = start.pre_tag = start.cur_tag = start_tag;
		start.score = 0;

		for (int i = 0; i <= sentencelength + 1; i++)
			MaxScore[i].clear();

		MaxScore[0].push_back(start);
		
		std::vector<TTS> q;
		TTS tmp;
		
		for (pos = 1; pos <= sentencelength; pos++)
		{
			q.clear();
			for (cur_tag = 1; cur_tag <= tag_size; cur_tag++)
				for (auto itr = MaxScore[pos-1].begin(); itr != MaxScore[pos-1].end(); itr++)
				{
					tmp.pre_pre_tag = itr->pre_tag;
					tmp.pre_tag = itr->cur_tag;
					tmp.cur_tag = cur_tag;
					tmp.score = itr->score + get_score(pos, tmp.pre_pre_tag, tmp.pre_tag, tmp.cur_tag, test);
					q.push_back(tmp);
				}
			std::sort(q.begin(), q.end());
			for (int i = 0; i < BEAM_SIZE && i < q.size(); i++)
				MaxScore[pos].push_back(q[i]);							
		}
			

		pos = sentencelength + 1;
		cur_tag = end_tag;
		for (auto itr = MaxScore[pos - 1].begin(); itr != MaxScore[pos - 1].end(); itr++)
		{
			tmp.pre_pre_tag = itr->pre_tag;
			tmp.pre_tag = itr->cur_tag;
			tmp.cur_tag = cur_tag;
			tmp.score = itr->score + get_score(pos, tmp.pre_pre_tag, tmp.pre_tag, tmp.cur_tag, test);
			q.push_back(tmp);
		}
		std::sort(q.begin(), q.end());
		
		tmp = q[0];
		if (tmp.score < -1000) std::cout<<"negative  "<<testround<<std::endl;
		for (pos = sentencelength; pos >= 1; pos--)
		{
			tagintsentence[pos] = tmp.pre_tag;
			for (auto itr = MaxScore[pos].begin(); itr != MaxScore[pos].end(); itr++)
			{
				if (itr->cur_tag == tmp.pre_tag && itr->pre_tag == tmp.pre_pre_tag)
				{
					tmp = *itr;
					break;
				}					
			}
		}

		int errflag = 0;
		int debug = 0;
		for (pos = 1; pos <= sentencelength; pos++)
			if (tagintsentence[pos] != correct_tagintsentence[pos])
			{
				if (debug)
				{
					std::cout << stringwordsentence[pos - 1] << ' ' << stringwordsentence[pos] << ' ' << stringwordsentence[pos + 1] << std::endl;
					std::cout << m_Weight->map_postags.getTokenizer().key(tagintsentence[pos - 1]) << ' ' << m_Weight->map_postags.getTokenizer().key(tagintsentence[pos]) << std::endl;
					std::cout << m_Weight->map_postags.getTokenizer().key(correct_tagintsentence[pos - 1]) << ' ' << m_Weight->map_postags.getTokenizer().key(correct_tagintsentence[pos]) << std::endl;
				}
				++totaltagerr;
				errflag = 1;
			}
		if (errflag) m_nTotalErrors++;
		if (errflag && debug) system("pause");

		if (bTrain) {
			int times = 0;
			for (pos = 1; pos <= sentencelength + 1; pos++)
				 if (tagintsentence[pos] != correct_tagintsentence[pos] || tagintsentence[pos - 1] != correct_tagintsentence[pos - 1] || (pos >=2 && tagintsentence[pos - 2] != correct_tagintsentence[pos - 2]))
				{
					getOrUpdateStackScore(pos, pos - 2 >= 0 ? correct_tagintsentence[pos - 2] : start_tag, correct_tagintsentence[pos - 1], correct_tagintsentence[pos], 1);
					getOrUpdateStackScore(pos, pos - 2 >= 0 ? tagintsentence[pos - 2] : start_tag, tagintsentence[pos - 1], tagintsentence[pos], -1);
				}			

		}
		else {
			for (pos = 1; pos <= sentencelength; pos++) {
				retval->push_back(m_Weight->map_postags.getTokenizer().key(tagintsentence[pos]));
			}
		}
	}
	
}
