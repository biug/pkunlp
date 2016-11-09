#include "stdafx.h"
#include <memory>
#include <string>
#include <fstream>
#include <iostream>
#include <ctime>
#include <algorithm>
#include <sstream>
#include "seg_run.h"
#include "seg_depparser.h"
#include "seg_macros.h"
#include "common/token/word.h"

namespace Segment {

	void Run::initParser(const std::string & sFeatureInput, const std::string & sFeatureOutput, const std::string & sDictPath, bool parse) {
	
		m_pSegmentor.reset(new DepParser(sFeatureInput, sFeatureOutput, sDictPath, !parse));
	}

	void Run::train(const std::string & sInputFile , int e) const {

		int nRound = 0;
		WordSentence wordsentence;
		TagSentence tagsentence;
		WordTagged wordtaggedsentence;

		std::ifstream input(sInputFile);

		std::vector<WordTagged> ran;
		while (input >> wordtaggedsentence)
			ran.push_back(wordtaggedsentence);
		random_shuffle(ran.begin(),ran.end());

		if (e == CP_ACP) {// UTF8 Encoding
			for (auto & sent : ran) {
				for (auto & word : sent.wordsentence)
					word = GBK2UTF8(word);
			}
		}

		if (ran.size() > 0) {
			for(auto tmpwordtaggedsentence : ran ) {
				//std::cout << nRound << std::endl;
				wordsentence = tmpwordtaggedsentence.wordsentence;
				++nRound;
				m_pSegmentor->train(wordsentence , nRound);				
			}
			m_pSegmentor->finishtraining();
		}
		input.close();

		std::cout << "Done." << std::endl;
	}

	void Run::parse(const std::string & sInputFile, const std::string & sOutputFile, int e) const {

		TagSentence tagsentence;

		std::ifstream input(sInputFile);
		std::ofstream output(sOutputFile);

		static std::string period = GBK2UTF8("¡£");

		if (input) {
			std::string line;
			while (std::getline( input , line )) {

				if (line.empty()) break;
				if (line.size() < MAX_SEG_SIZE) {
					if (e == CP_ACP) line = GBK2UTF8(line);		// ANSI Encoding
					
					int pos = -1;
					while ((pos = line.find(period)) != std::string::npos && pos != line.size() - period.size()) {
						tagsentence.clear();
						m_pSegmentor->parse(line.substr(0, pos + period.size()), &tagsentence);
						line = line.substr(pos + period.size());
						output << tagsentence << ' ';
					}
					
					tagsentence.clear();
					m_pSegmentor->parse(line, &tagsentence);
					output << tagsentence << std::endl;
				}
			}
		}
		input.close();
		output.close();
	}

	std::string Run::parse(const std::string & sInput, int e) const {
		std::string result = "";
		if (sInput.empty()) return "";
		TagSentence tagsentence;
		std::string line = sInput;
		if (e == CP_ACP) line = GBK2UTF8(line);
		int pos = -1;
		static std::string period = GBK2UTF8("¡£");

		while ((pos = line.find(period)) != std::string::npos && pos != line.size() - period.size()) {
			tagsentence.clear();
			m_pSegmentor->parse(line.substr(0, pos + period.size()), &tagsentence);
			line = line.substr(pos + period.size());
			for (int i = 0, n = tagsentence.size(); i < n; ++i) result += tagsentence[i] + ' ';
		}

		tagsentence.clear();
		m_pSegmentor->parse(line, &tagsentence);
		result += tagsentence[0];
		for (int i = 1, n = tagsentence.size(); i < n; ++i) result += ' ' + tagsentence[i];

		return result;
	}

	std::vector<std::pair<std::string, std::string>> Run::parse(const std::vector<std::string> & vecInput, int) const {
		return {};
	}
}
