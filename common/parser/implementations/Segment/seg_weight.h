#ifndef _Segment_WEIGHT_H
#define _Segment_WEIGHT_H

#include <unordered_map>
#include "seg_macros.h"
#include "common/oldtoken/word.h"
#include "common/oldtoken/pos.h"
#include "common/parser/old_weight_base.h"
#include "include/learning/perceptron/packed_score.h"

namespace Segment {
	class Weight {
	protected:
		bool m_bTrain;
		std::string m_sReadPath;
		std::string m_sRecordPath;
		std::string m_sDictPath;

	public:

		TWordOld map_words;
		TPOSTagOld map_postags;

	public:

		PosWordTagIntMap map_pos_char_tag_int;	
		PosWordWordTagIntMap map_pos_char_char_tag_int;
		PosWordWordTagIntMap map_pos_prechar_nextchar_tag_int;

		PosIdenTagIntMap map_pos_idc_tag_int1;
		PosIdenTagIntMap map_pos_idc_tag_int2;

		PosIdenTagIntMap map_pos_idw_tag_int1;
		PosIdenTagIntMap map_pos_idw_tag_int2;

		PosPosIdenTagIntMap map_pos_pos_idw_tag_int1;
		PosPosIdenTagIntMap map_pos_pos_idw_tag_int2;
		PosPosIdenTagIntMap map_pos_pos_dic_tag_int;
		TagTagIntMap map_tag_tag_int;

		std::unordered_map<std::string , std::vector<std::string> > map_dic_int;


	public:
		Weight(const std::string & sRead, const std::string & sRecord, const std::string & sDict, bool bTrain);
		~Weight();

		void loadScores();
		void saveScores() const;
		void loadDicScores();
		void debug();
		void computeAverageFeatureWeights(const int & round);
		std::vector<std::string> getdic(std::string word)
		{
			std::vector<std::string> Nullvec;
			Nullvec.clear();
			if (map_dic_int.find(word) != map_dic_int.end())
				return map_dic_int[word];
			else return Nullvec;
		}
	};
}

#endif
