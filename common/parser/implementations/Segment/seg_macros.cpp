#include "stdafx.h"
#include "seg_macros.h"

namespace Segment {

	std::istream & operator >> (std::istream & input, WordTagged & sentence) {
		sentence.wordsentence.clear();
		sentence.tagsentence.clear();
		std::stringstream s;
		std::string word;
		std::string tag;
		std::string wordtag;
		int i = 0;
		while (std::getline(input, wordtag))
		{
			if (wordtag.empty()) break;
			//std::cout << word << std::endl;//debug
			s << wordtag;
			s >> word;
			if (s)
				s >> tag;
			s.clear();
			//std::cout << wordtag << std::endl;
			//std::cout << ++i<<' '<<word << "_" << tag << std::endl;
			sentence.wordsentence.push_back(word);
			sentence.tagsentence.push_back(tag);
		}
		//std::cout << sentence.wordsentence.size() << std::endl;
		return input;
	}

	std::ostream & operator<<(std::ostream & output, const WordTagged & sentence) {

		for (int i = 0; i < sentence.wordsentence.size(); i++) {
			output << sentence.wordsentence[i] << "/" << sentence.tagsentence[i] << std::endl;
		}
		output << std::endl;
		return output;
	}

	std::istream & operator >> (std::istream & input, WordSentence & sentence) {
		sentence.clear();
		std::string line, token;
		std::getline(input, line);
		std::istringstream iss(line);
		while (iss >> token && token != ".") {
			sentence.push_back(token);
		}
		return input;
	}

	std::ostream & operator<<(std::ostream & output, const TagSentence & tagsentence) {
		output << *tagsentence.begin();
		for (auto itr = tagsentence.begin() + 1; itr != tagsentence.end(); ++itr) {
			output << ' ' << *itr;
		}
		return output;
	}
}