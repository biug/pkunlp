#include "stdafx.h"
#include <memory>
#include <string>
#include <fstream>
#include <iostream>
#include <ctime>
#include <algorithm>
#include <sstream>
#include "postag_run.h"
#include "postag_depparser.h"
#include "postag_macros.h"
#include "common/token/word.h"

namespace POSTagging {

	void Run::initParser(const std::string & sFeatureInput, const std::string & sFeatureOutput, const std::string & sDictPath, bool parse) {
		m_pPOSTagger.reset(new DepParser(sFeatureInput, sFeatureOutput, !parse));
	}

	void Run::train(const std::string & sInputFile, int e) const {

		int nRound = 0;
		WordSentence wordsentence;
		TagSentence tagsentence;
		WordTagged wordtaggedsentence;
		std::ifstream input(sInputFile);

		std::vector<WordTagged> train_in;
		std::vector<int> R;
		train_in.clear();
		int idx = 0;
		if (input)
		{
			while (input >> wordtaggedsentence)
				train_in.push_back(wordtaggedsentence), R.push_back(idx++);
		}
		if (e == CP_ACP) {// UTF8 Encoding
			for (auto & sent : train_in) {
				for (auto & word : sent.wordsentence)
					word = GBK2UTF8(word);	
				for (auto & tag : sent.tagsentence)
					tag = GBK2UTF8(tag);
			}
		}
		std::cout << idx << std::endl;
		std::random_shuffle(R.begin(), R.end());

		if (idx) {
			for (int i = 0; i < idx; i++) {
				wordtaggedsentence = train_in[R[i]];
				wordsentence = wordtaggedsentence.wordsentence;
				tagsentence = wordtaggedsentence.tagsentence;
				++nRound;
				m_pPOSTagger->train(wordsentence , tagsentence , nRound);
			}
			m_pPOSTagger->finishtraining();
		}
		input.close();

		std::cout << "Done." << std::endl;
	}

	void Run::parse(const std::string & sInputFile, const std::string & sOutputFile, int e) const {

		WordSentence wordsentence;
		TagSentence tagsentence;
		TagSentence correct;

		std::ifstream input(sInputFile);
		std::ofstream output(sOutputFile);

		if (input) {
			std::string line, word;
			while (std::getline(input, line)) {
				if (line.empty()) break;
				if (e == CP_ACP) line = GBK2UTF8(line);	// UTF8 Encoding
				std::stringstream ss(line);
				wordsentence.clear();
				tagsentence.clear();
				correct.clear();
				while(ss >> word) {
					wordsentence.push_back(word);
					correct.push_back(word);
				}
				if (wordsentence.size() < MAX_SEG_SIZE) {
					m_pPOSTagger->parse(wordsentence, correct ,&tagsentence);

					output << wordsentence[0] << '/' << tagsentence[0];
					for (int i = 1, n = wordsentence.size(); i < n; ++i) output << ' ' << wordsentence[i] << '/' << tagsentence[i];
					output << std::endl;
				}
			}
		}
//		parser->finishparsing();
		input.close();
		output.close();
	}

	std::string Run::parse(const std::string & sInput, int e) const {
		std::string result = "";
		if (sInput.empty()) return "";
		std::string input = sInput;
		if (e == CP_ACP) input = GBK2UTF8(input);	// UTF8 Encoding
		static std::string period = "。";
		std::stringstream ss(sInput);
		WordSentence wordsentence;
		TagSentence tagsentence, correct;
		std::string word;
		while (ss >> word) {
			wordsentence.push_back(word);
			correct.push_back(word);
			if (word == period) {
				if (wordsentence.size() >= MAX_SEG_SIZE) return "";
				m_pPOSTagger->parse(wordsentence, correct, &tagsentence);
				for (int i = 1, n = wordsentence.size(); i < n; ++i) result += ' ' + wordsentence[i] + '/' + tagsentence[i];
				wordsentence.clear();
				tagsentence.clear();
				correct.clear();
			}
		}
		return result.substr(1);
	}

	std::vector<std::pair<std::string, std::string>> Run::parse(const std::vector<std::string> & vecInput, int e) const {
		std::vector<std::pair<std::string, std::string>> result;
		if (vecInput.empty()) return {};
		if (vecInput.size() >= MAX_SEG_SIZE) return {};
		WordSentence wordsentence;
		TagSentence tagsentence, correct;
		std::string word;
		static std::string period = "。";
		for (const auto & word : vecInput) {
			if (e == CP_ACP) {
				wordsentence.push_back(GBK2UTF8(word));
				correct.push_back(GBK2UTF8(word));
			}
			else {
				wordsentence.push_back(word);
				correct.push_back(word);
			}
			if (wordsentence.back() == period) {
				if(wordsentence.size() >= MAX_SEG_SIZE) return {};
				m_pPOSTagger->parse(wordsentence, correct, &tagsentence);
				for (int i = 0, n = wordsentence.size(); i < n; ++i) {
					result.push_back({ wordsentence[i], tagsentence[i] });
				}
				wordsentence.clear();
				tagsentence.clear();
				correct.clear();
			}
		}
		return result;
	}

}
