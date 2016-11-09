#include "stdafx.h"

#include <ctime>
#include <memory>
#include <fstream>
#include <iostream>

#include "arceager_run.h"
#include "arceager_depparser.h"

namespace arceager {

	Run::Run() = default;

	Run::~Run() = default;

	void Run::train(const std::string & sInputFile, const std::string & sFeatureInput, const std::string & sFeatureOutput) {
		int nRound = 0;
		DependencyTree ref_sent;

		std::cout << "Training iteration is started..." << std::endl;

		std::unique_ptr<DepParser> parser(new DepParser(sFeatureInput, sFeatureOutput, ParserState::TRAIN));
		parser->m_tLabels.add("ROOT");

		std::ifstream input(sInputFile);
		if (input) {
			while (input >> ref_sent) {
				for (const auto & token : ref_sent) {
					parser->m_tLabels.add(TREENODE_LABEL(token));
				}
			}
		}
		input.close();

		parser->m_AC.AL_FIRST = POP_ROOT + 1;
		parser->m_AC.AL_END = parser->m_AC.AR_FIRST = parser->m_AC.AL_FIRST + parser->m_tLabels.count();
		parser->m_AC.AR_END = parser->m_AC.AR_FIRST + parser->m_tLabels.count();
		input.open(sInputFile);
		if (input) {
			while (input >> ref_sent) {
				++nRound;
				parser->train(ref_sent, nRound);
			}
			parser->finishtraining();
		}
		input.close();

		std::cout << "Done." << std::endl;
	}

	void Run::parse(const std::string & sInputFile, const std::string & sOutputFile, const std::string & sFeatureFile) {

		Sentence sentence;
		DependencyTree tree;

		std::cout << "Parsing started" << std::endl;

		std::unique_ptr<DepParser> parser(new DepParser(sFeatureFile, sFeatureFile, ParserState::PARSE));
		std::ifstream input(sInputFile);
		std::ofstream output(sOutputFile);

		parser->m_AC.AL_FIRST = POP_ROOT + 1;
		parser->m_AC.AL_END = parser->m_AC.AR_FIRST = parser->m_AC.AL_FIRST + parser->m_tLabels.count();
		parser->m_AC.AR_END = parser->m_AC.AR_FIRST + parser->m_tLabels.count();

		if (input) {
			while (input >> sentence) {
				if (sentence.size() < MAX_SENTENCE_SIZE) {
					parser->parse(sentence, &tree);
					output << tree;
					tree.clear();
				}
			}
		}
		input.close();
		output.close();
	}

	void Run::goldtest(const std::string & sInputFile, const std::string & sFeatureInput) {
		int nRound = 0;
		DependencyTree ref_sent;

		std::cout << "GoldTest iteration is started..." << std::endl;

		std::unique_ptr<DepParser> parser(new DepParser(sFeatureInput, "", ParserState::GOLDTEST));
		parser->m_tLabels.add("ROOT");

		std::ifstream input(sInputFile);
		if (input) {
			while (input >> ref_sent) {
				for (const auto & token : ref_sent) {
					parser->m_tLabels.add(TREENODE_LABEL(token));
				}
			}
		}
		input.close();

		parser->m_AC.AL_FIRST = POP_ROOT + 1;
		parser->m_AC.AL_END = parser->m_AC.AR_FIRST = parser->m_AC.AL_FIRST + parser->m_tLabels.count();
		parser->m_AC.AR_END = parser->m_AC.AR_FIRST + parser->m_tLabels.count();

		input.open(sInputFile);
		if (input) {
			while (input >> ref_sent) {
				++nRound;
				parser->goldCheck(ref_sent);
			}
		}
		input.close();

		std::cout << "total " << nRound << " round" << std::endl;

		std::cout << "Done." << std::endl;
	}
}
