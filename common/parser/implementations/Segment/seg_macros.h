#ifndef _Segment_MACROS_H
#define _Segment_MACROS_H

#include <sstream>
#include "common/parser/macros_base.h"
#include "include/learning/perceptron/packed_score.h"

namespace Segment {
	#define EISNER_PACKED_SIZE 1
	#define MAX_SEG_SIZE 2000

	template <int PACKED_SIZE>
	class PackedScoreType {
	protected:
		tscore scores[PACKED_SIZE];

	public:
		PackedScoreType() = default;
		~PackedScoreType() = default;

		tscore & operator[](const std::size_t & n) {
			return scores[n];
		}

		tscore & operator[](const std::size_t & n) const {
			return scores[n];
		}

		void reset() {
			memset(scores, 0, sizeof(tscore) * PACKED_SIZE);
		}

		bool empty() const {
			for (int i = 0; i < PACKED_SIZE; ++i) {
				if (scores[i] != 0) {
					return false;
				}
			}
			return true;
		}

		void add(const PackedScoreType & score) {
			for (int i = 0; i < PACKED_SIZE; ++i) {
				scores[i] += score[i];
			}
		}
	};

	template <int PACKED_SIZE>
	class PackedScore {

	private:
		std::unordered_map<int, Score> scores;

	public:
		PackedScore() = default;
		~PackedScore() = default;

		tscore score(const int & index, const int & n) {
			if (scores.find(index) == scores.end()) {
				return 0;
			}
			return scores[index][n];
		}

		void updateCurrent(const int & index, const int & added, const int & round) {
			//std::cout << "index is " << index << " added is " << added << " round is " << round << std::endl;
			scores[index].updateCurrent(added, round);
		}

		void updateAverage(const int & round) {
			for (auto itr = scores.begin(); itr != scores.end(); ++itr) {
				itr->second.updateAverage(round);
			}
		}

		void reset() {
			for (auto itr = scores.begin(); itr != scores.end(); ++itr) {
				itr->second.reset();
			}
		}

		bool empty() const {
			for (auto itr = scores.begin(); itr != scores.end(); ++itr) {
				if (!itr->second.zero()) {
					return false;
				}
			}
			return true;
		}

		void add(PackedScoreType<PACKED_SIZE> & pst, int which) {
			for (auto itr = scores.begin(); itr != scores.end(); ++itr) {
				pst[itr->first] += itr->second[which];
			}
		}

		friend std::istream & operator >> (std::istream & is, PackedScore<PACKED_SIZE> & ps) {
			int index;
			Score score;
			std::string token;

			ps.scores.clear();
			is >> token;
			while (true) {
				is >> index >> token >> score >> token;
				ps.scores[index] = score;
				if (token == "}") {
					break;
				}
			}
			return is;
		}

		friend std::ostream & operator<<(std::ostream & os, const PackedScore<PACKED_SIZE> & ps) {
			os << "{ ";
			bool write;
			std::unordered_map<int, Score>::const_iterator itr = ps.scores.begin();
			while (true) {
				if ((write = !itr->second.zero())) {
					os << itr->first << " : " << itr->second;
				}
				if (++itr != ps.scores.end()) {
					if (write) {
						os << " , ";
					}
				}
				else {
					os << " }";
					break;
				}
			}
			return os;
		}
	};

	template<typename KEY_TYPE, int PACKED_SIZE>
	class PackedScoreMap {
	private:
		std::unordered_map < KEY_TYPE, PackedScore<PACKED_SIZE> > map;
		std::string name;
		int count;

	public:
		PackedScoreMap(const std::string & input_name = "") : name(input_name), count(0) {}
		~PackedScoreMap() = default;

		void getScore(PackedScoreType<PACKED_SIZE> pst, const KEY_TYPE & key, int which) {
			if (map.find(key) != map.end()) {
				map[key].add(pst, which);
			}
		}

		void updateScore(const KEY_TYPE & key, const int & index, const int & amount, const int & round) {
			map[key].updateCurrent(index, amount, round);
		}

		void getOrUpdateScore(PackedScoreType<PACKED_SIZE> & out, const KEY_TYPE & key, const int & index, const int & which, const int & amount = 0, const int & round = 0) {
			if (amount == 0) {
				if (map.find(key) != map.end()) {
					map[key].add(out, which);
				}
			}
			else {
				updateScore(key, index, amount, round);
			}
		}

		void computeAverage(const int & round) {
			count = 0;
			for (auto itr = map.begin(); itr != map.end(); ++itr) {
				itr->second.updateAverage(round);
				if (!itr->second.empty()) {
					++count;
				}
			}
		}

		void clearScore() {
			for (auto itr = map.begin(); itr != map.end(); ++itr) {
				itr->second.reset();
			}
		}

		void print(const KEY_TYPE & key) const {
			if (map.find(key) != map.end()) {
				std::cout << map.at(key) << std::endl;
			}
			else {
				std::cout << "no key" << std::endl;
			}
		}

		void print() const {
			std::cout << *this << std::endl;
		}

		friend std::istream& operator >> (std::istream & is, PackedScoreMap<KEY_TYPE, PACKED_SIZE> & psm) {
			int number;
			KEY_TYPE key;
			std::string token;
			PackedScore<PACKED_SIZE> ps;

			psm.map.clear();
			is >> psm.name >> psm.count;
			number = psm.count;
			while (number--) {
				is >> key >> token >> ps;
				psm.map.insert({ key, ps });
			}
			return is;
		}

		friend std::ostream& operator<<(std::ostream & os, const PackedScoreMap<KEY_TYPE, PACKED_SIZE> & psm) {
			os << psm.name << " " << psm.count << std::endl;
			//std::cout << psm.map.size() << " " << psm.count << std::endl;
			for (auto itr = psm.map.begin(); itr != psm.map.end(); ++itr) {
				if (!itr->second.empty()) {
					os << itr->first << " : " << itr->second << std::endl;
				}
			}
			return os;
		}
	};

	typedef gtype Char;

	typedef QuarGram<gtype> PosWordTagInt;
	typedef QuarGram<gtype> PosIdenTagInt;
	typedef QuinGram<gtype> PosWordWordTagInt;
	typedef QuinGram<gtype> PosPosIdenTagInt;
	typedef TriGram<gtype> TagTagInt;

	typedef PackedScoreMap<PosWordTagInt, EISNER_PACKED_SIZE> PosWordTagIntMap;
	typedef PackedScoreMap<PosIdenTagInt, EISNER_PACKED_SIZE> PosIdenTagIntMap;
	typedef PackedScoreMap<PosWordWordTagInt, EISNER_PACKED_SIZE> PosWordWordTagIntMap;
	typedef PackedScoreMap<PosPosIdenTagInt, EISNER_PACKED_SIZE> PosPosIdenTagIntMap;
	typedef PackedScoreMap<TagTagInt, EISNER_PACKED_SIZE> TagTagIntMap;

	std::istream & operator>>(std::istream & input, WordTagged & sentence);
	std::ostream & operator<<(std::ostream & output, const WordTagged & sentence);
	std::istream & operator>>(std::istream & input, WordSentence & sentence);
	std::ostream & operator<<(std::ostream & output, const TagSentence & tagsentence);
}

#endif
