#include "stdafx.h"
#include <cmath>
#include <stack>
#include <string>
#include <map>
#include <set>
#include <ctype.h>
#include <vector>
#include <unordered_set>

#include "seg_depparser.h"
#include "common/oldtoken/word.h"
#include "common/oldtoken/pos.h"
#include "seg_macros.h"
#include <regex>

#define INF 0x3fffffff
#define NUM_LEN 3
#define ENG_LEN 3
#define NORM_STATE -1
#define NUM_STATE 0
#define ENG_STATE 1

namespace Segment {
	POSTag DepParser::empty_tag= POSTag();
	POSTag DepParser::start_tag = POSTag();
	POSTag DepParser::end_tag = POSTag();
	Word DepParser::empty_word = Word();
	Word DepParser::start_word = Word();
	Word DepParser::end_word = Word();
	Char START_CHAR = 2;
	Char EMPTY_CHAR = 2;
	Char END_CHAR = 2;
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

	int tmpcharsentence[MAX_SEG_SIZE];

	std::string makestring(int l, int r)
	{
		std::string word;
		word.clear();
		std::stack<int> s;
		while (!s.empty()) s.pop();
		for (int i = l; i <= r; i++)
		{
			int x = tmpcharsentence[i];
			int base = 256 * 256;
			int flag = 0;
			while (base)
			{
				if (x > base || flag)
					word.push_back(x / base), flag = 1;
				x %= base;
				base /= 256;
			}
		}
		return word;
	}
	int totaltagcor;
	std::map<TT, int> map_test;
	DepParser::DepParser(const std::string & sFeatureInput, const std::string & sFeatureOut, const std::string & sDictPath, bool bTrain) :
		m_bTrain(bTrain) {

		maxlen = 0;
		maxtag = 0;
		testround = 0;
		sentencelength = 0;
		m_nTotalErrors = 0;
		m_nTrainingRound = 0;
		totaltag = 0;
		totaltagerr = 0;
		totaltagcor = 0;
		m_nScoreIndex = bTrain ? eNonAverage : eAverage;

		m_Weight = new Weight(sFeatureInput, sFeatureOut, sDictPath, bTrain);
		end_tag = start_tag = m_Weight->map_postags.code("S");
		//std::cout << start_tag << ' ' << end_tag << std::endl;
		m_Weight->map_postags.code("B");
		m_Weight->map_postags.code("I");
		m_Weight->map_postags.code("E");
		
		m_Weight->map_words.code(START_WORD);
		m_Weight->map_words.code(EMPTY_WORD);
		m_Weight->map_words.code(END_WORD);
		
		/*第 15183020
		零 15309750
		一 14989440
		二 14989964
		三 14989449
		四 15047579
		五 14989972
		六 15041965
		七 14989443
		八 15041963
		九 14989725
		十 15043969
		百 15178174
		千 15043971
		万 14989447
		亿 14990015
		兆 15041926
		点 15172281
		负 15250591
		两 14989476
		0  '0'	
		1  '1'
		2  '2'
		3  '3'
		4  '4'
		5  '5' 
		6  '6'
		7  '7'
		8  '8'
		9  '9'
		.  '.'
		% '%'
		*/
		Char num[] = { 15183020,15309750,14989440,14989964,14989449,15047579,
					14989972,15041965,14989443,15041963,14989725,15043969,15178174,
					15043971,14989447,14990015,15041926,15172281,15250591,14989476,
					'0','1', '2', '3', '4', '5', '6', '7', '8', '9', '.','%'};
		num_set.insert(num, num + sizeof(num) / sizeof(num[0]));
		
		Char pun[] = { '#','(',')','[',']','{','}','<','>',';','-',':' };

		pun_set.insert(pun, pun + sizeof(pun) / sizeof(pun[0]));
	}

	DepParser::~DepParser() {
		delete m_Weight;
	}

	tscore DepParser::get_score(int position, POSTag pre_tag , POSTag cur_tag,bool test) {
		
		if (test)
		{
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
			getOrUpdateStackScore(position, pre_tag , cur_tag);
			return packed_scores[0];
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

	void DepParser::toMyString_UTF8(std::string s) // change string to charsentence[]
	{		
		int i;
		unsigned int num = 0;
		int st = 0;

		if (s == START_WORD)
		{
			sentencelength++;
			tmpcharsentence[sentencelength] = charsentence[sentencelength] = START_CHAR;
			correct_tagintsentence[sentencelength] = 1;
			return;
		}
		else if (s == EMPTY_WORD)
		{
			sentencelength++;
			tmpcharsentence[sentencelength] = charsentence[sentencelength] = EMPTY_CHAR;
			correct_tagintsentence[sentencelength] = 1;
			return;
		}
		else if (s == END_WORD)
		{
			sentencelength++;
			tmpcharsentence[sentencelength] = charsentence[sentencelength] = END_CHAR;
			correct_tagintsentence[sentencelength] = 1;
			return;
		}
		
		for (i = 0; i < s.size(); i++)
		{
			unsigned char ch = s[i];
			unsigned char x = ch;

			if (getpreone(ch) != 1)
			{
				if (num != 0) charsentence[++sentencelength] = num;
				num = ch;
			}					
			else				
				num = num * 256 + ch;
		}
		charsentence[++sentencelength] = num;
	} 

	int toUTF8(std::string s)
	{
		int i;
		unsigned int num = 0;
		int st = 0;

		for (i = 0; i < s.size(); i++)
		{
			unsigned char ch = s[i];
			unsigned char x = ch;
			num = num * 256 + ch;
		}
		return num;
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

	void DepParser::train(const WordSentence & ref_sent , int round) {
		// initialize
		int df = 0;
		int test = false;
		WordSentence half_sent;
		for (auto & sent : ref_sent)
			half_sent.push_back(toBanJiao(sent));

		sentencelength = -1;
		
		toMyString_UTF8(START_WORD);
		std::string pre = makestring(0, 0),now;
		for (auto itr = half_sent.begin(); itr != half_sent.end(); ++itr) {
			int l = sentencelength;
			toMyString_UTF8(*itr);
			int r = sentencelength;
			if (l + 1 == r) correct_tagintsentence[r] = m_Weight->map_postags.code("S");
			else
			{
				correct_tagintsentence[l + 1] = m_Weight->map_postags.code("B");
				correct_tagintsentence[r] = m_Weight->map_postags.code("E");
				for (int i = l + 2; i < r; i++)
					correct_tagintsentence[i] = m_Weight->map_postags.code("I");
			}
			now = makestring(l + 1, r);
			m_Weight->map_words.code(*itr);
			//TWordOld::code(pre + "T.T" + now);
			pre = now;
		}
		toMyString_UTF8(END_WORD);
		now = makestring(sentencelength, sentencelength);

		// train
		work(nullptr, true,test);
		m_nTrainingRound++;
		
		//system("pause");
		totaltag += ref_sent.size();
		if (m_Weight->map_postags.getTokenizer().count() >= 5)
		{
			std::cout << round << std::endl;
			std::cout << m_Weight->map_postags.getTokenizer().count() << std::endl;
			system("pause");
		}

		if (m_nTrainingRound % 1000 == 0) {
			std::cout << m_nTotalErrors << " / " << m_nTrainingRound <<' '<< 1 - 1.0*m_nTotalErrors / m_nTrainingRound << std::endl;
			std::cout << totaltagcor << " / " << totaltag << ' ' << 1.0 * totaltagcor / totaltag << std::endl;
			//std::cout << maxlen << ' ' << maxtag << std::endl;
		}
	}

	void DepParser::finishparsing()
	{
		std::cout << testround << " all done\n";
		//std::cout << m_nTotalErrors << " / " << testround << ' ' << 1 - 1.0*m_nTotalErrors / testround << std::endl;
		//std::cout << totaltagcor << " / " << totaltag << ' ' << 1.0 * totaltagcor / totaltag << std::endl;
	}

	void DepParser::finishtraining() {
		
		Weight * cweight = (Weight*)m_Weight;
		cweight->computeAverageFeatureWeights(m_nTrainingRound);
		cweight->saveScores();
		std::cout << m_nTotalErrors << " / " << m_nTrainingRound <<' '<< 1 - 1.0*m_nTotalErrors / m_nTrainingRound << std::endl;
		std::cout << totaltagcor << " / " << totaltag << ' ' << 1.0 * totaltagcor / totaltag << std::endl;
		std::cout << m_Weight->map_words.getTokenizer().count() << std::endl;
		std::cout << m_Weight->map_postags.getTokenizer().count() << std::endl;

	}

	void DepParser::segsentence(std::vector<std::vector<Char> > & sent_list) {
		std::vector<Char> sent;
		int fup = 14844060, fdown = 14844061;// “ ”半角
		int fju = 14909570, fwen = 15711391, ftan = 15711361;// 。？！半角
		int flag = 0;
		for (int i = 0; i <= sentencelength; i++) {
			Char ch = charsentence[i];
			sent.push_back(ch);
			if (ch == fup)
				flag++;
			else if (ch == fdown)
				flag--;
			if ( flag == 0 && ( ch == fju || ch == fwen || ch == ftan) ){
				sent_list.push_back(sent);
				sent.clear();
			}				
		}
		if (!sent.empty()) {
			sent_list.push_back(sent);
			sent.clear();
		}			
	}
	
	void DepParser::parse(const std::string & ref_sent, TagSentence * retval) {

		int idx = 0;
		/*for (auto ch : ref_sent)
			std::cout << (int)ch << ' ';
		std::cout << std::endl;
*/
		std::string half_sent = toBanJiao(ref_sent);
		/*for (auto ch : half_sent)
			std::cout << (int)ch << ' ';
		std::cout << std::endl;*/

		m_nTrainingRound = 0;
		sentencelength = -1;
		testround++;

		toMyString_UTF8(half_sent);
				
		//seg sentence
		std::vector<std::vector<Char> > sent_list;
		segsentence(sent_list);
		for (auto & sent : sent_list)
		{
			sentencelength = -1;
			charsentence[++sentencelength] = START_CHAR;
			for (auto & ch : sent)
				charsentence[++sentencelength] = ch;
			charsentence[++sentencelength] = EMPTY_CHAR;
			work(retval, false, false);
			//std::cout << "xx" << std::endl;
		}			
		
		totaltag += half_sent.size();
	}

	void DepParser::getOrUpdateStackScore(int position, POSTag pre_tag, POSTag cur_tag , int amount) {//缁熼敓鏂ゆ嫹feature vector

		Weight * cweight = (Weight*)m_Weight;

		tag_tag_int.refer(pre_tag, cur_tag, 0);
		cweight->map_tag_tag_int.getOrUpdateScore(packed_scores, tag_tag_int, 0, m_nScoreIndex, amount, m_nTrainingRound);
		int flag;

		for (int i = -5; i <= 5; i++)
		{
			if (position + i < 0 || position + i > sentencelength) tmpcharsentence[i+5] = EMPTY_CHAR;
			else tmpcharsentence[i+5] = charsentence[position + i];
		}
//unigram
		for (int s = -2; s <= 2; s++)
		{
			pos_char_tag_int.refer(s,tmpcharsentence[s + 5], cur_tag, 0);
			cweight->map_pos_char_tag_int.getOrUpdateScore(packed_scores, pos_char_tag_int, 0, m_nScoreIndex, amount, m_nTrainingRound);
		}
//bigram
		for (int s = -2; s <= 2; s++)
		{
			pos_char_char_tag_int.refer(s, tmpcharsentence[s + 5], tmpcharsentence[s + 6], cur_tag, 0);
			cweight->map_pos_char_char_tag_int.getOrUpdateScore(packed_scores, pos_char_char_tag_int, 0, m_nScoreIndex, amount, m_nTrainingRound);
		}
//jump
		pos_char_char_tag_int.refer(0, tmpcharsentence[4], tmpcharsentence[6], cur_tag, 0);
		cweight->map_pos_prechar_nextchar_tag_int.getOrUpdateScore(packed_scores, pos_char_char_tag_int, 0, m_nScoreIndex, amount, m_nTrainingRound);
//ci = ci+1
		for (int s = -1; s <= 1; s++)
		{
			flag = tmpcharsentence[s + 5] == tmpcharsentence[s + 6] ? 1 : 0;
			if (flag > 0)
			{
				pos_idc_tag_int1.refer(s, flag, cur_tag, 0);
				cweight->map_pos_idc_tag_int1.getOrUpdateScore(packed_scores, pos_idc_tag_int1, 0, m_nScoreIndex, amount, m_nTrainingRound);
			}
		}
//ci = ci+2
		for (int s = -3; s <= 1; s++)
		{
			flag = tmpcharsentence[s + 5] == tmpcharsentence[s + 7] ? 1 : 0;
			if (flag > 0)
			{
				pos_idc_tag_int2.refer(s, flag, cur_tag, 0);
				cweight->map_pos_idc_tag_int2.getOrUpdateScore(packed_scores, pos_idc_tag_int1, 0, m_nScoreIndex, amount, m_nTrainingRound);
			}
		}
		
		for (int s = -5; s < 0; s++)
		{
			flag = m_Weight->map_words.getTokenizer().find(makestring(s + 5, 5));
			//flag = TWordOld::code(makestring(s + 5, 5));
			//if (flag != 0 && amount == 0 && cur_tag == 1 && pre_tag == 1 ) std::cout << s << ' '<< position << ' '<<flag<<std::endl;
			if (flag > 0)
			{
				pos_idw_tag_int1.refer(s, flag, cur_tag, 0);
				cweight->map_pos_idw_tag_int1.getOrUpdateScore(packed_scores, pos_idw_tag_int1, 0, m_nScoreIndex, amount, m_nTrainingRound);
			}
		}

		for (int e = 1; e < 6; e++)
		{
			flag = m_Weight->map_words.getTokenizer().find(makestring(5, e + 5));
			//flag = TWordOld::code(makestring(5, e + 5));
			if (flag > 0)
			{
				pos_idw_tag_int2.refer(e, flag, cur_tag, 0);
				cweight->map_pos_idw_tag_int2.getOrUpdateScore(packed_scores, pos_idw_tag_int2, 0, m_nScoreIndex, amount, m_nTrainingRound);
			}
		}

		for (int i = 1 ; i <= 4 ; i++)
		{
			for (auto dic : cweight->getdic(makestring(5,5+i)))
			{
				int flag = m_Weight->map_words.getTokenizer().find(dic);
				if (flag > 0)
				pos_pos_dic_tag_int.refer(5,5+i, flag, cur_tag, 0),
				cweight->map_pos_pos_dic_tag_int.getOrUpdateScore(packed_scores, pos_pos_dic_tag_int, 0, m_nScoreIndex, amount, m_nTrainingRound);

			}
		}

		for (int i = 1 ; i <= 4 ; i++)
		{
			for (auto dic : cweight->getdic(makestring(5-i,5)))
			{
				int flag = m_Weight->map_words.getTokenizer().find(dic);
				if (flag > 0)
				pos_pos_dic_tag_int.refer(5-i,5, flag, cur_tag, 0),
				cweight->map_pos_pos_dic_tag_int.getOrUpdateScore(packed_scores, pos_pos_dic_tag_int, 0, m_nScoreIndex, amount, m_nTrainingRound);
			}
		}

	}

	void DepParser::prework(){
		
		int l = -1, r;	
		int state = NORM_STATE;//-1 other 0 num 1 eng 
		for (int i = 1; i <= sentencelength; i++){
			int ch = charsentence[i];
			//分 15042694 之 14989707
			if (state == NORM_STATE) {
				if (num_set.find(ch) != num_set.end())
					state = NUM_STATE, l = i;
				else if (ch < 128 && ch != ' ')
					state = ENG_STATE, l = i;
				else state = NORM_STATE;
			}
			else if (state == NUM_STATE) {
				if (ch == 15042694 && charsentence[i + 1] == 14989707){ 
					i++; continue;
				}					
				if (num_set.find(ch) != num_set.end() )
					state = NUM_STATE;
				else {
					if (i - l >= NUM_LEN) {
						for (int j = l + 1; j < i - 1; j++)
							tagintsentence[j] = m_Weight->map_postags.code("I");
					}
					if (ch < 128 && ch != ' ') 
						state = ENG_STATE,l = i;					
					else 
						state = NORM_STATE;					
				}
			}
			else if (state == ENG_STATE) {
				if (ch < 128 && ch != ' ' && i != sentencelength)
					state = ENG_STATE;
				else {
					if (i - l >= ENG_LEN) {
						for (int j = l + 1; j < i - 1; j++)
							tagintsentence[j] = m_Weight->map_postags.code("I");
						if (charsentence[i - 1] == '.' || charsentence[i - 1] == '?' || charsentence[i - 1] == '!')
							tagintsentence[i - 2] = -1;
					}
					if (num_set.find(ch) != num_set.end()) 
						state = NUM_STATE, l = i;					
					else 
						state = NORM_STATE;					
				}				
			}
		}
		for (int i = 1; i <= sentencelength; i++) {
			int ch = charsentence[i];
			if ( pun_set.find(ch) != pun_set.end() && tagintsentence[i] == -1) {
				tagintsentence[i] = m_Weight->map_postags.code("S");
				if (tagintsentence[i - 1] != m_Weight->map_postags.code("S"))
					tagintsentence[i - 1] = -1;
				if (tagintsentence[i + 1] != m_Weight->map_postags.code("S"))
					tagintsentence[i + 1] = -1;
			}
		}
		
	}

	void DepParser::work(TagSentence * retval, bool bTrain,bool test) {

		int pos;
		POSTag pre_tag, cur_tag;
		int tag_size = m_Weight->map_postags.getTokenizer().count();

		std::vector<std::vector<tscore> > MaxScore(sentencelength + 10,
			std::vector<tscore>(MAX_SEGTAG_SIZE));
		std::vector<std::vector<POSTag> > PreTag(sentencelength + 10,
			std::vector<POSTag>(MAX_SEGTAG_SIZE));
		memset(tagintsentence, -1, sizeof(tagintsentence));
		prework();
		//memset(tagintsentence, -1, sizeof(tagintsentence));
		tagintsentence[0] = m_Weight->map_postags.code("S");
		tagintsentence[sentencelength] = m_Weight->map_postags.code("S");
		
		MaxScore[0][1] = 0;
		for (int i = 2; i <= tag_size; i++)
			MaxScore[0][i] = - INF;

		for (pos = 1; pos <= sentencelength; pos++){			
			for (cur_tag = 1; cur_tag <= tag_size; cur_tag++) {
				for (pre_tag = 1; pre_tag <= tag_size; pre_tag++) {
					tscore tmpscore = MaxScore[pos - 1][pre_tag] + get_score(pos, pre_tag, cur_tag, test);
					if (pre_tag == 1 || MaxScore[pos][cur_tag] < tmpscore) {
						PreTag[pos][cur_tag] = pre_tag;
						MaxScore[pos][cur_tag] = tmpscore;
					}
				}

				//std::cout << "pos = " << pos << " cur_tag = " << cur_tag << " max score = " << MaxScore[pos][cur_tag] << std::endl;
			}
			if (tagintsentence[pos] != -1)
				for (cur_tag = 1; cur_tag <= tag_size; cur_tag++)
					if (cur_tag != tagintsentence[pos])
						MaxScore[pos][cur_tag] = -INF;

		}					

		cur_tag = 1;
		for (pos = sentencelength ; pos > 1; pos--)
			cur_tag = (tagintsentence[pos - 1] = PreTag[pos][cur_tag]);

		int errflag = 0;
		int debug = 0;
		for (pos = 1; pos < sentencelength; pos++)
			if (tagintsentence[pos] != correct_tagintsentence[pos])
			{
				if (debug)
				{
					//std::cout << stringwordsentence[pos - 1] << ' ' << stringwordsentence[pos] << ' ' << stringwordsentence[pos + 1] << std::endl;
					std::cout << m_Weight->map_postags.getTokenizer().key(tagintsentence[pos - 1]) << ' ' << m_Weight->map_postags.getTokenizer().key(tagintsentence[pos]) << std::endl;
					std::cout << m_Weight->map_postags.getTokenizer().key(correct_tagintsentence[pos - 1]) << ' ' << m_Weight->map_postags.getTokenizer().key(correct_tagintsentence[pos]) << std::endl;
				}
				//++totaltagerr;
				errflag = 1;
			}
		int cor = 0, my = 0;

		for (pos = 1; pos < sentencelength; pos++)
		{
			if ((correct_tagintsentence[pos] == 1 || correct_tagintsentence[pos] == 4) && (tagintsentence[pos] == 1 || tagintsentence[pos] == 4))
			{
				if ( cor == my )
					totaltagcor++;
			}
			if (correct_tagintsentence[pos] == 1 || correct_tagintsentence[pos] == 4)
				cor = pos;
			if (tagintsentence[pos] == 1 || tagintsentence[pos] == 4)
			{
				my = pos ;
			}				
		}

		if (errflag) m_nTotalErrors++;
		if (errflag && debug) system("pause");
		
		for (int i = 1; i < sentencelength; i++)
			tmpcharsentence[i] = charsentence[i];
		if (bTrain) {
			int times = 0;
			for (pos = 1; pos <= sentencelength; pos++)
				 if (tagintsentence[pos] != correct_tagintsentence[pos] || tagintsentence[pos - 1] != correct_tagintsentence[pos - 1])
				{
					//std::cout << pos << std::endl;
					getOrUpdateStackScore(pos, correct_tagintsentence[pos - 1], correct_tagintsentence[pos], 1);
					getOrUpdateStackScore(pos, tagintsentence[pos - 1], tagintsentence[pos], -1);
				}
		}
		else {
			int l = 0;
			/*for (pos = 1; pos < sentencelength; pos++)
				std::cout << tagintsentence[pos] << ' ';
			std::cout << std::endl;*/
			for (pos = 1; pos < sentencelength; pos++)
			{
				//tagintsentence[pos] = 1;
				if (tagintsentence[pos] == 1)
					retval->push_back(makestring(pos, pos));
				else if (tagintsentence[pos] == 2)
				{
					if (tagintsentence[pos + 1] == 2 || tagintsentence[pos + 1] == 1) {
						tagintsentence[pos] = 1; pos--; continue;
					}
					l = pos;
				}					
				else if (tagintsentence[pos] == 3)
				{
					if (tagintsentence[pos - 1] == 1 || tagintsentence[pos - 1] == 4){
						tagintsentence[pos] = 2; pos--; continue;
					}
					if (tagintsentence[pos + 1] == 1 || tagintsentence[pos + 1] == 2) {
						tagintsentence[pos] = 4; pos--; continue;
					}
				}
				else if (tagintsentence[pos] == 4)
				{
					if (tagintsentence[pos - 1] == 1 || tagintsentence[pos - 1] == 4) {
						tagintsentence[pos] = 1; pos--; continue;
					}
					retval->push_back(makestring(l, pos));
				}					
			}		

		}
	}

	void DepParser::toMyString_ANSI(std::string s)
	{
		if (s == START_WORD)
		{
			sentencelength++;
			tmpcharsentence[sentencelength] = charsentence[sentencelength] = START_CHAR;
			correct_tagintsentence[sentencelength] = 1;
			return;
		}
		else if (s == EMPTY_WORD)
		{
			sentencelength++;
			tmpcharsentence[sentencelength] = charsentence[sentencelength] = EMPTY_CHAR;
			correct_tagintsentence[sentencelength] = 1;
			return;
		}
		else if (s == END_WORD)
		{
			sentencelength++;
			tmpcharsentence[sentencelength] = charsentence[sentencelength] = END_CHAR;
			correct_tagintsentence[sentencelength] = 1;
			return;
		}

		int i, num = 0;
		int st = 0;

		for (i = 0; i < s.size(); i++)
		{

			if (s[i] > 0)
				sentencelength++, tmpcharsentence[sentencelength] = charsentence[sentencelength] = s[i];
			else
			{
				if (st == 0)
					num = s[i];
				else if (st == 1)
				{
					num = num + s[i] * 256;
					sentencelength++;
					tmpcharsentence[sentencelength] = charsentence[sentencelength] = num;
				}
				st = st ^ 1;
			}
		}

	}
}
