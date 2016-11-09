#include "stdafx.h"
#include <cmath>
#include <stack>
#include <cstring>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <unordered_set>

#include "arceager_depparser.h"

std::string toHalfWidth(const std::string & input);

namespace arceager {

	WordPOSTag DepParser::empty_taggedword = WordPOSTag();
	SetOfDepLabels DepParser::empty_setoflabels = SetOfDepLabels();

	DepParser::DepParser(const std::string &sFeatureInput, const std::string &sFeatureOutput, int nState) :
			DepParserBase(nState), m_tWords(1), m_tPOSTags(1), m_tLabels(1) {

		m_nSentenceLength = 0;

		m_Weight = new Weight(sFeatureInput, sFeatureOutput, &m_tWords, &m_tPOSTags, &m_tLabels);

		DepParser::empty_taggedword.refer(m_tWords.lookup(EMPTY_WORD), m_tPOSTags.lookup(EMPTY_POSTAG));
		empty_setoflabels.add(0);
	}

	DepParser::~DepParser() {
		delete m_Weight;
	}

	void DepParser::train(const DependencyTree &correct, const int &round) {
		// initialize
		int idx = 0;
		int lastTotalErrors = m_nTotalErrors;
		int lastTrainingRound = m_nTrainingRound;
		m_nTrainingRound = round;
		m_nSentenceLength = correct.size();
		for (const auto &node : correct) {
			m_lSentence[idx++].refer(m_tWords.lookup(TREENODE_WORD(node)), m_tPOSTags.lookup(TREENODE_POSTAG(node)));
		}

		// train
		work(nullptr, correct);
		if (lastTrainingRound > 0) {
			nBackSpace("error rate 0.0000 ( " + std::to_string(lastTotalErrors) + " / " +
					   std::to_string(lastTrainingRound) + " ) ");
		}
		if (m_nTrainingRound > 0) {
			std::cout << "error rate " << ((double) m_nTotalErrors / (double) m_nTrainingRound);
			std::cout << " ( " << m_nTotalErrors << " / " << m_nTrainingRound << " ) " << std::flush;
		}
	}

	void DepParser::parse(const Sentence &sentence, DependencyTree *retval, bool print) {
		int idx = 0;
		++m_nTrainingRound;
		DependencyTree correct;
		m_nSentenceLength = sentence.size();
		for (const auto &token : sentence) {
			m_lSentence[idx++].refer(m_tWords.lookup(SENT_WORD(token)), m_tPOSTags.lookup(SENT_POSTAG(token)));
			correct.push_back(DependencyTreeNode(token, -1, NULL_LABEL));
		}
		work(retval, correct);
		if (print) {
			if (m_nTrainingRound > 1) {
				nBackSpace("parsing sentence " + std::to_string(m_nTrainingRound - 1));
			}
			std::cout << "parsing sentence " << m_nTrainingRound << std::flush;
		}
	}

	void DepParser::goldCheck(const DependencyTree &correct) {
		StateItem output;
		while (output.standardMove(correct, m_tLabels, m_AC)) {
			std::cout << output.lastAction() << std::endl;
		}
		int action;
		StateItem input;
		while ((action = input.followMove(output, m_AC)) != POP_ROOT) {
			input.move(action, m_AC);
		}
		input.move(action, m_AC);
		if (input != output) {
			std::cout << "error follow move" << std::endl;
		}
	}

	void DepParser::work(DependencyTree *retval, const DependencyTree &correct) {

		m_abItems[0].clear();
		m_abItems[1].clear();
		m_iCandidate.clear();
		m_iCorrect.clear();

		m_pGenerator = &m_abItems[0];
		m_pGenerated = &m_abItems[1];

		m_pGenerator->insertItem(m_iCandidate);

		for (int index = 0, len = (m_nSentenceLength << 1); index < len; ++index) {

			decode();

			// early update check
			if (m_nState == TRAIN) {
				bool bCorrect = false;

				for (const auto &item : *m_pGenerator) {

					if (*item == m_iCorrect) {
						bCorrect = true;
						break;
					}
				}
				// early update
				if (!bCorrect) {
					update();
					return;
				}
				m_iCorrect.standardMove(correct, m_tLabels, m_AC);
			}

			std::swap(m_pGenerated, m_pGenerator);
		}

		switch (m_nState) {
			case ParserState::TRAIN:
				update();
				break;
			case ParserState::PARSE:
				generate(retval, correct);
				break;
			default:
				break;
		}
	}

	void DepParser::decode() {

		m_pGenerated->clear();

		for (const auto &iGenerator : *m_pGenerator) {
			m_abScores.clear();
			getActionScores(*iGenerator);

			const tscore &score = iGenerator->getScore();

			if (iGenerator->size() == m_nSentenceLength) {
				if (iGenerator->stackBack() > 0) {
					reduce(score);
				} else {
					popRoot(score);
				}
			} else {
				if (!iGenerator->afterReduce()) {
					if (iGenerator->size() < m_nSentenceLength - 1 || iGenerator->stackEmpty()) {
						shift(score);
					}
				}
				if (!iGenerator->stackEmpty()) {
					if (iGenerator->size() < m_nSentenceLength - 1 || iGenerator->headStackBack() == 0) {
						arcRight(score);
					}
					if (iGenerator->head(iGenerator->stackTop()) != -1) {
						reduce(score);
					} else {
						arcLeft(score);
					}
				}
			}

			for (const auto &saScore : m_abScores) {
				m_iCandidate = *iGenerator;
				m_iCandidate.setScore(saScore->getScore());
				m_iCandidate.move(saScore->getAction(), m_AC);
				m_pGenerated->insertItem(m_iCandidate);
			}
		}
	}

	void DepParser::update() {

		m_iStatesItem.clear();
		const StateItem &output = m_pGenerator->bestUnsortItem();

		if (output != m_iCorrect) {
			while (m_iStatesItem != output) {
				int action = m_iStatesItem.followMove(output, m_AC);
				int correct_action = m_iStatesItem.followMove(m_iCorrect, m_AC);
				if (action == correct_action) {
					m_iStatesItem.move(action, m_AC);
				} else {
					break;
				}
			}
			updateScoreForState(m_iStatesItem, m_iCorrect, 1);
			updateScoreForState(m_iStatesItem, output, -1);
			++m_nTotalErrors;
		}
	}

	void DepParser::generate(DependencyTree *retval, const DependencyTree &correct) {
		m_pGenerator->bestUnsortItem().generateTree(correct, *retval, m_tLabels);
	}

	void DepParser::getActionScores(const StateItem &item) {
		memset(m_mapPackedScore, 0, sizeof(tscore) * MAX_ACTION);
		getOrUpdateStackScore(item, std::make_pair<int, const int &>(NO_ACTION, 0));
	}

	void DepParser::updateScoreForState(const StateItem &from, const StateItem &output, const int &amount) {
		m_iStateItem = from;
		std::pair<int, int> m_pairAmount(NO_ACTION, amount);
		while (m_iStateItem != output) {
			int action = m_iStateItem.followMove(output, m_AC);
			m_pairAmount.first = action;
			getOrUpdateStackScore(m_iStateItem, m_pairAmount);
			m_iStateItem.move(action, m_AC);
		}
	}

	void DepParser::getOrUpdateStackScore(const StateItem &item, const ActionScoreIncrement &amount) {

		Weight *cweight = (Weight *) m_Weight;

		const int &st_index = item.stackEmpty() ? -1 : item.stackTop();
		const int &sth_index = st_index == -1 ? -1 : item.head(st_index);
		const int &sthh_index = sth_index == -1 ? -1 : item.head(sth_index);
		const int &stld_index = st_index == -1 ? -1 : item.leftDep(st_index);
		const int &strd_index = st_index == -1 ? -1 : item.rightDep(st_index);
		const int &stl2d_index = stld_index == -1 ? -1 : item.sibling(stld_index);
		const int &str2d_index = strd_index == -1 ? -1 : item.sibling(strd_index);
		const int &n0_index = ((item.size() == m_nSentenceLength) ? -1 : item.size());
		const int &n0ld_index = n0_index == -1 ? -1 : item.leftDep(n0_index);
		const int &n0l2d_index = n0ld_index == -1 ? -1 : item.sibling(n0ld_index);
		const int &s1_index = ((st_index + 1 < m_nSentenceLength) ? st_index + 1 : -1);
		const int &s2_index = ((st_index + 2 < m_nSentenceLength) ? st_index + 2 : -1);
		const int &n1_index = ((n0_index + 1 < m_nSentenceLength) ? n0_index + 1 : -1);
		const int &n2_index = ((n0_index + 2 < m_nSentenceLength) ? n0_index + 2 : -1);

		const WordPOSTag &st_word_tag = st_index == -1 ? empty_taggedword : m_lSentence[st_index];
		const WordPOSTag &sth_word_tag = sth_index == -1 ? empty_taggedword : m_lSentence[sth_index];
		const WordPOSTag &sthh_word_tag = sthh_index == -1 ? empty_taggedword : m_lSentence[sthh_index];
		const WordPOSTag &stld_word_tag = stld_index == -1 ? empty_taggedword : m_lSentence[stld_index];
		const WordPOSTag &strd_word_tag = strd_index == -1 ? empty_taggedword : m_lSentence[strd_index];
		const WordPOSTag &stl2d_word_tag = stl2d_index == -1 ? empty_taggedword : m_lSentence[stl2d_index];
		const WordPOSTag &str2d_word_tag = str2d_index == -1 ? empty_taggedword : m_lSentence[str2d_index];
		const WordPOSTag &n0_word_tag = n0_index == -1 ? empty_taggedword : m_lSentence[n0_index];
		const WordPOSTag &n0ld_word_tag = n0ld_index == -1 ? empty_taggedword : m_lSentence[n0ld_index];
		const WordPOSTag &n0l2d_word_tag = n0l2d_index == -1 ? empty_taggedword : m_lSentence[n0l2d_index];
		const WordPOSTag &s1_word_tag = s1_index == -1 ? empty_taggedword : m_lSentence[s1_index];
		const WordPOSTag &s2_word_tag = s2_index == -1 ? empty_taggedword : m_lSentence[s2_index];
		const WordPOSTag &n1_word_tag = n1_index == -1 ? empty_taggedword : m_lSentence[n1_index];
		const WordPOSTag &n2_word_tag = n2_index == -1 ? empty_taggedword : m_lSentence[n2_index];

		const Word &st_word = st_word_tag.first();
		const Word &sth_word = sth_word_tag.first();
		const Word &sthh_word = sthh_word_tag.first();
		const Word &stld_word = stld_word_tag.first();
		const Word &strd_word = strd_word_tag.first();
		const Word &stl2d_word = stl2d_word_tag.first();
		const Word &str2d_word = str2d_word_tag.first();
		const Word &n0_word = n0_word_tag.first();
		const Word &n0ld_word = n0ld_word_tag.first();
		const Word &n0l2d_word = n0l2d_word_tag.first();
		const Word &s1_word = s1_word_tag.first();
		const Word &s2_word = s2_word_tag.first();
		const Word &n1_word = n1_word_tag.first();
		const Word &n2_word = n2_word_tag.first();

		const POSTag &st_tag = st_word_tag.second();
		const POSTag &sth_tag = sth_word_tag.second();
		const POSTag &sthh_tag = sthh_word_tag.second();
		const POSTag &stld_tag = stld_word_tag.second();
		const POSTag &strd_tag = strd_word_tag.second();
		const POSTag &stl2d_tag = stl2d_word_tag.second();
		const POSTag &str2d_tag = str2d_word_tag.second();
		const POSTag &n0_tag = n0_word_tag.second();
		const POSTag &n0ld_tag = n0ld_word_tag.second();
		const POSTag &n0l2d_tag = n0l2d_word_tag.second();
		const POSTag &s1_tag = s1_word_tag.second();
		const POSTag &s2_tag = s2_word_tag.second();
		const POSTag &n1_tag = n1_word_tag.second();
		const POSTag &n2_tag = n2_word_tag.second();

		const int &st_label = st_index == -1 ? 0 : item.label(st_index);
		const int &sth_label = sth_index == -1 ? 0 : item.label(sth_index);
		const int &stld_label = stld_index == -1 ? 0 : item.label(stld_index);
		const int &strd_label = strd_index == -1 ? 0 : item.label(strd_index);
		const int &stl2d_label = stl2d_index == -1 ? 0 : item.label(stl2d_index);
		const int &str2d_label = str2d_index == -1 ? 0 : item.label(str2d_index); //PROBLEM!
		const int &n0ld_label = n0ld_index == -1 ? 0 : item.label(n0ld_index);
		const int &n0l2d_label = n0l2d_index == -1 ? 0 : item.label(n0l2d_index);

		const int &st_n0_dist = encodeLinkDistanceOrDirection(st_index, n0_index, false);

		const int &st_rarity = st_index == -1 ? 0 : item.rightArity(st_index);
		const int &st_larity = st_index == -1 ? 0 : item.leftArity(st_index);
		const int &n0_larity = n0_index == -1 ? 0 : item.leftArity(n0_index);

		const SetOfDepLabels &st_rtagset =
				st_index == -1 ? empty_setoflabels : SetOfDepLabels(item.rightSetOfLabels(st_index));
		const SetOfDepLabels &st_ltagset =
				st_index == -1 ? empty_setoflabels : SetOfDepLabels(item.leftSetOfLabels(st_index));
		const SetOfDepLabels &n0_ltagset =
				n0_index == -1 ? empty_setoflabels : SetOfDepLabels(item.leftSetOfLabels(n0_index));

		if (st_index != -1) {
			cweight->m_mapSTw.getOrUpdateScore(m_mapPackedScore, st_word, m_nScoreIndex, amount, m_nTrainingRound);
			cweight->m_mapSTt.getOrUpdateScore(m_mapPackedScore, st_tag, m_nScoreIndex, amount, m_nTrainingRound);
			word_tag.refer(st_word, st_tag);
			cweight->m_mapSTwt.getOrUpdateScore(m_mapPackedScore, word_tag, m_nScoreIndex, amount, m_nTrainingRound);
		}

		if (n0_index != -1) {
			cweight->m_mapN0w.getOrUpdateScore(m_mapPackedScore, n0_word, m_nScoreIndex, amount, m_nTrainingRound);
			cweight->m_mapN0t.getOrUpdateScore(m_mapPackedScore, n0_tag, m_nScoreIndex, amount, m_nTrainingRound);
			word_tag.refer(n0_word, n0_tag);
			cweight->m_mapN0wt.getOrUpdateScore(m_mapPackedScore, word_tag, m_nScoreIndex, amount, m_nTrainingRound);
		}

		if (s1_index != -1) {
			cweight->m_mapS1w.getOrUpdateScore(m_mapPackedScore, s1_word, m_nScoreIndex, amount, m_nTrainingRound);
			cweight->m_mapS1t.getOrUpdateScore(m_mapPackedScore, s1_tag, m_nScoreIndex, amount, m_nTrainingRound);
			word_tag.refer(s1_word, s1_tag);
			cweight->m_mapS1wt.getOrUpdateScore(m_mapPackedScore, word_tag, m_nScoreIndex, amount, m_nTrainingRound);
		}

		if (s2_index != -1) {
			cweight->m_mapS2w.getOrUpdateScore(m_mapPackedScore, s2_word, m_nScoreIndex, amount, m_nTrainingRound);
			cweight->m_mapS2t.getOrUpdateScore(m_mapPackedScore, s2_tag, m_nScoreIndex, amount, m_nTrainingRound);
			word_tag.refer(s2_word, s2_tag);
			cweight->m_mapS2wt.getOrUpdateScore(m_mapPackedScore, word_tag, m_nScoreIndex, amount, m_nTrainingRound);
		}

		if (n1_index != -1) {
			cweight->m_mapN1w.getOrUpdateScore(m_mapPackedScore, n1_word, m_nScoreIndex, amount, m_nTrainingRound);
			cweight->m_mapN1t.getOrUpdateScore(m_mapPackedScore, n1_tag, m_nScoreIndex, amount, m_nTrainingRound);
			word_tag.refer(n1_word, n1_tag);
			cweight->m_mapN1wt.getOrUpdateScore(m_mapPackedScore, word_tag, m_nScoreIndex, amount, m_nTrainingRound);
		}

		if (n2_index != -1) {
			cweight->m_mapN2w.getOrUpdateScore(m_mapPackedScore, n2_word, m_nScoreIndex, amount, m_nTrainingRound);
			cweight->m_mapN2t.getOrUpdateScore(m_mapPackedScore, n2_tag, m_nScoreIndex, amount, m_nTrainingRound);
			word_tag.refer(n2_word, n2_tag);
			cweight->m_mapN2wt.getOrUpdateScore(m_mapPackedScore, word_tag, m_nScoreIndex, amount, m_nTrainingRound);
		}

		if (sth_index != -1) {
			cweight->m_mapSTHw.getOrUpdateScore(m_mapPackedScore, sth_word, m_nScoreIndex, amount, m_nTrainingRound);
			cweight->m_mapSTHt.getOrUpdateScore(m_mapPackedScore, sth_tag, m_nScoreIndex, amount, m_nTrainingRound);
			cweight->m_mapSTi.getOrUpdateScore(m_mapPackedScore, st_label, m_nScoreIndex, amount, m_nTrainingRound);
		}

		if (sthh_index != -1) {
			cweight->m_mapSTHHw.getOrUpdateScore(m_mapPackedScore, sthh_word, m_nScoreIndex, amount, m_nTrainingRound);
			cweight->m_mapSTHHt.getOrUpdateScore(m_mapPackedScore, sthh_tag, m_nScoreIndex, amount, m_nTrainingRound);
			cweight->m_mapSTHi.getOrUpdateScore(m_mapPackedScore, sth_label, m_nScoreIndex, amount, m_nTrainingRound);
		}

		if (stld_index != -1) {
			cweight->m_mapSTLDw.getOrUpdateScore(m_mapPackedScore, stld_word, m_nScoreIndex, amount, m_nTrainingRound);
			cweight->m_mapSTLDt.getOrUpdateScore(m_mapPackedScore, stld_tag, m_nScoreIndex, amount, m_nTrainingRound);
			cweight->m_mapSTLDi.getOrUpdateScore(m_mapPackedScore, stld_label, m_nScoreIndex, amount, m_nTrainingRound);
		}

		if (strd_index != -1) {
			cweight->m_mapSTRDw.getOrUpdateScore(m_mapPackedScore, strd_word, m_nScoreIndex, amount, m_nTrainingRound);
			cweight->m_mapSTRDt.getOrUpdateScore(m_mapPackedScore, strd_tag, m_nScoreIndex, amount, m_nTrainingRound);
			cweight->m_mapSTRDi.getOrUpdateScore(m_mapPackedScore, strd_label, m_nScoreIndex, amount, m_nTrainingRound);
		}

		if (n0ld_index != -1) {
			cweight->m_mapN0LDw.getOrUpdateScore(m_mapPackedScore, n0ld_word, m_nScoreIndex, amount, m_nTrainingRound);
			cweight->m_mapN0LDt.getOrUpdateScore(m_mapPackedScore, n0ld_tag, m_nScoreIndex, amount, m_nTrainingRound);
			cweight->m_mapN0LDi.getOrUpdateScore(m_mapPackedScore, n0ld_label, m_nScoreIndex, amount, m_nTrainingRound);
		}

		if (stl2d_index != -1) {
			cweight->m_mapSTL2Dw.getOrUpdateScore(m_mapPackedScore, stl2d_word, m_nScoreIndex, amount,
												  m_nTrainingRound);
			cweight->m_mapSTL2Dt.getOrUpdateScore(m_mapPackedScore, stl2d_tag, m_nScoreIndex, amount, m_nTrainingRound);
			cweight->m_mapSTL2Di.getOrUpdateScore(m_mapPackedScore, stl2d_label, m_nScoreIndex, amount,
												  m_nTrainingRound);
		}

		if (str2d_index != -1) {
			cweight->m_mapSTR2Dw.getOrUpdateScore(m_mapPackedScore, str2d_word, m_nScoreIndex, amount,
												  m_nTrainingRound);
			cweight->m_mapSTR2Dt.getOrUpdateScore(m_mapPackedScore, str2d_tag, m_nScoreIndex, amount, m_nTrainingRound);
			cweight->m_mapSTR2Di.getOrUpdateScore(m_mapPackedScore, str2d_label, m_nScoreIndex, amount,
												  m_nTrainingRound);
		}

		if (n0l2d_index != -1) {
			cweight->m_mapN0L2Dw.getOrUpdateScore(m_mapPackedScore, n0l2d_word, m_nScoreIndex, amount,
												  m_nTrainingRound);
			cweight->m_mapN0L2Dt.getOrUpdateScore(m_mapPackedScore, n0l2d_tag, m_nScoreIndex, amount, m_nTrainingRound);
			cweight->m_mapN0L2Di.getOrUpdateScore(m_mapPackedScore, n0l2d_label, m_nScoreIndex, amount,
												  m_nTrainingRound);
		}

		if (st_index != -1) {
			st_word_tag_n0_word_tag.refer(st_word, n0_word, st_tag, n0_tag);
			cweight->m_mapSTwtN0wt.getOrUpdateScore(m_mapPackedScore, st_word_tag_n0_word_tag, m_nScoreIndex, amount,
													m_nTrainingRound);
			word_word_tag.refer(st_word, n0_word, st_tag);
			cweight->m_mapSTwtN0w.getOrUpdateScore(m_mapPackedScore, word_word_tag, m_nScoreIndex, amount,
												   m_nTrainingRound);
			word_word_tag.refer(st_word, n0_word, n0_tag);
			cweight->m_mapSTwN0wt.getOrUpdateScore(m_mapPackedScore, word_word_tag, m_nScoreIndex, amount,
												   m_nTrainingRound);
			word_tag_tag.refer(st_word, st_tag, n0_tag);
			cweight->m_mapSTwtN0t.getOrUpdateScore(m_mapPackedScore, word_tag_tag, m_nScoreIndex, amount,
												   m_nTrainingRound);
			word_tag_tag.refer(n0_word, st_tag, n0_tag);
			cweight->m_mapSTtN0wt.getOrUpdateScore(m_mapPackedScore, word_tag_tag, m_nScoreIndex, amount,
												   m_nTrainingRound);
			st_word_n0_word.refer(st_word, n0_word);
			cweight->m_mapSTwN0w.getOrUpdateScore(m_mapPackedScore, st_word_n0_word, m_nScoreIndex, amount,
												  m_nTrainingRound);
			set_of_2_tags = ENCODE_POSTAG_SET_2(st_tag, n0_tag);
			cweight->m_mapSTtN0t.getOrUpdateScore(m_mapPackedScore, set_of_2_tags, m_nScoreIndex, amount,
												  m_nTrainingRound);
		}

		if (st_index != -1 && n0_index != -1) {
			set_of_2_tags = ENCODE_POSTAG_SET_2(n0_tag, n1_tag);
			cweight->m_mapN0tN1t.getOrUpdateScore(m_mapPackedScore, set_of_2_tags, m_nScoreIndex, amount,
												  m_nTrainingRound);
			set_of_3_tags = ENCODE_POSTAG_SET_3(n0_tag, n1_tag, n2_tag);
			cweight->m_mapN0tN1tN2t.getOrUpdateScore(m_mapPackedScore, set_of_3_tags, m_nScoreIndex, amount,
													 m_nTrainingRound);
			set_of_3_tags = ENCODE_POSTAG_SET_3(st_tag, n0_tag, n1_tag);
			cweight->m_mapSTtN0tN1t.getOrUpdateScore(m_mapPackedScore, set_of_3_tags, m_nScoreIndex, amount,
													 m_nTrainingRound);
			set_of_3_tags = ENCODE_POSTAG_SET_3(st_tag, n0_tag, n0ld_tag);
			cweight->m_mapSTtN0tN0LDt.getOrUpdateScore(m_mapPackedScore, set_of_3_tags, m_nScoreIndex, amount,
													   m_nTrainingRound);
			set_of_3_tags = ENCODE_POSTAG_SET_3(n0_tag, n0ld_tag, n0l2d_tag);
			cweight->m_mapN0tN0LDtN0L2Dt.getOrUpdateScore(m_mapPackedScore, set_of_3_tags, m_nScoreIndex, amount,
														  m_nTrainingRound);
		}

		if (st_index != -1) {
			set_of_3_tags = ENCODE_POSTAG_SET_3(sth_tag, st_tag, n0_tag);
			cweight->m_mapSTHtSTtN0t.getOrUpdateScore(m_mapPackedScore, set_of_3_tags, m_nScoreIndex, amount,
													  m_nTrainingRound);
			set_of_3_tags = ENCODE_POSTAG_SET_3(sthh_tag, sth_tag, st_tag);
			cweight->m_mapSTHHtSTHtSTt.getOrUpdateScore(m_mapPackedScore, set_of_3_tags, m_nScoreIndex, amount,
														m_nTrainingRound);
			set_of_3_tags = ENCODE_POSTAG_SET_3(st_tag, stld_tag, n0_tag);
			cweight->m_mapSTtSTLDtN0t.getOrUpdateScore(m_mapPackedScore, set_of_3_tags, m_nScoreIndex, amount,
													   m_nTrainingRound);
			set_of_3_tags = ENCODE_POSTAG_SET_3(st_tag, stld_tag, stl2d_tag);
			cweight->m_mapSTtSTLDtSTL2Dt.getOrUpdateScore(m_mapPackedScore, set_of_3_tags, m_nScoreIndex, amount,
														  m_nTrainingRound);
			set_of_3_tags = ENCODE_POSTAG_SET_3(st_tag, strd_tag, n0_tag);
			cweight->m_mapSTtSTRDtN0t.getOrUpdateScore(m_mapPackedScore, set_of_3_tags, m_nScoreIndex, amount,
													   m_nTrainingRound);
			set_of_3_tags = ENCODE_POSTAG_SET_3(st_tag, strd_tag, str2d_tag);
			cweight->m_mapSTtSTRDtSTR2Dt.getOrUpdateScore(m_mapPackedScore, set_of_3_tags, m_nScoreIndex, amount,
														  m_nTrainingRound);
		}

		if (st_index != -1 && n0_index != -1) {
			word_int.refer(st_word, st_n0_dist);
			cweight->m_mapSTwd.getOrUpdateScore(m_mapPackedScore, word_int, m_nScoreIndex, amount, m_nTrainingRound);
			tag_int.refer(st_tag, st_n0_dist);
			cweight->m_mapSTtd.getOrUpdateScore(m_mapPackedScore, tag_int, m_nScoreIndex, amount, m_nTrainingRound);
			word_int.refer(n0_word, st_n0_dist);
			cweight->m_mapN0wd.getOrUpdateScore(m_mapPackedScore, word_int, m_nScoreIndex, amount, m_nTrainingRound);
			tag_int.refer(n0_tag, st_n0_dist);
			cweight->m_mapN0td.getOrUpdateScore(m_mapPackedScore, tag_int, m_nScoreIndex, amount, m_nTrainingRound);
			word_word_int.refer(st_word, n0_word, st_n0_dist);
			cweight->m_mapSTwN0wd.getOrUpdateScore(m_mapPackedScore, word_word_int, m_nScoreIndex, amount,
												   m_nTrainingRound);
			tag_tag_int.refer(st_tag, n0_tag, st_n0_dist);
			cweight->m_mapSTtN0td.getOrUpdateScore(m_mapPackedScore, tag_tag_int, m_nScoreIndex, amount,
												   m_nTrainingRound);
		}

		if (st_index != -1) {
			word_int.refer(st_word, st_rarity);
			cweight->m_mapSTwra.getOrUpdateScore(m_mapPackedScore, word_int, m_nScoreIndex, amount, m_nTrainingRound);
			tag_int.refer(st_tag, st_rarity);
			cweight->m_mapSTtra.getOrUpdateScore(m_mapPackedScore, tag_int, m_nScoreIndex, amount, m_nTrainingRound);
			word_int.refer(st_word, st_larity);
			cweight->m_mapSTwla.getOrUpdateScore(m_mapPackedScore, word_int, m_nScoreIndex, amount, m_nTrainingRound);
			tag_int.refer(st_tag, st_larity);
			cweight->m_mapSTtla.getOrUpdateScore(m_mapPackedScore, tag_int, m_nScoreIndex, amount, m_nTrainingRound);
		}

		if (n0_index != -1) {
			word_int.refer(n0_word, n0_larity);
			cweight->m_mapN0wla.getOrUpdateScore(m_mapPackedScore, word_int, m_nScoreIndex, amount, m_nTrainingRound);
			tag_int.refer(n0_tag, n0_larity);
			cweight->m_mapN0tla.getOrUpdateScore(m_mapPackedScore, tag_int, m_nScoreIndex, amount, m_nTrainingRound);
		}

		if (st_index != -1) {
			word_tagset.refer(st_word, st_rtagset.bits(0), st_rtagset.bits(1), st_rtagset.bits(2));
			cweight->m_mapSTwrp.getOrUpdateScore(m_mapPackedScore, word_tagset, m_nScoreIndex, amount,
												 m_nTrainingRound);
			tag_tagset.refer(st_tag, st_rtagset.bits(0), st_rtagset.bits(1), st_rtagset.bits(2));
			cweight->m_mapSTtrp.getOrUpdateScore(m_mapPackedScore, tag_tagset, m_nScoreIndex, amount, m_nTrainingRound);
			word_tagset.refer(st_word, st_ltagset.bits(0), st_ltagset.bits(1), st_ltagset.bits(2));
			cweight->m_mapSTwlp.getOrUpdateScore(m_mapPackedScore, word_tagset, m_nScoreIndex, amount,
												 m_nTrainingRound);
			tag_tagset.refer(st_tag, st_ltagset.bits(0), st_ltagset.bits(1), st_ltagset.bits(2));
			cweight->m_mapSTtlp.getOrUpdateScore(m_mapPackedScore, tag_tagset, m_nScoreIndex, amount, m_nTrainingRound);
		}

		if (n0_index != -1) {
			word_tagset.refer(n0_word, n0_ltagset.bits(0), n0_ltagset.bits(1), n0_ltagset.bits(2));
			cweight->m_mapN0wlp.getOrUpdateScore(m_mapPackedScore, word_tagset, m_nScoreIndex, amount,
												 m_nTrainingRound);
			tag_tagset.refer(n0_tag, n0_ltagset.bits(0), n0_ltagset.bits(1), n0_ltagset.bits(2));
			cweight->m_mapN0tlp.getOrUpdateScore(m_mapPackedScore, tag_tagset, m_nScoreIndex, amount, m_nTrainingRound);
		}
	}

	void DepParser::ParseStream(std::istream &input, std::ostream &output, int encoding) {
		Sentence sentence;
		DependencyTree tree;

		if (input) {
			while (input >> sentence) {
				for (auto &&node : sentence) {
					SENT_WORD(node) = toHalfWidth(SENT_WORD(node));
					SENT_POSTAG(node) = toHalfWidth(SENT_POSTAG(node));
				}
				if (sentence.size() < MAX_SENTENCE_SIZE) {
					this->parse(sentence, &tree);
					output << tree;
					tree.clear();
				}
			}
			output << std::endl;
		}
	}

	void DepParser::parseFile(const std::string &input_file,
				   const std::string &output_file, int encoding) {
		std::ifstream input(input_file);
		std::ofstream output(output_file);

		this->ParseStream(input, output, encoding);

		input.close();
		output.close();
	}

	void DepParser::parseString(const std::string & input, int encoding) {
		std::stringstream is(input);
		std::stringbuf buffer;
		std::ostream os(&buffer);

		ParseStream(is, os, encoding);

		std::string tmp = buffer.str();
		tmp.swap(parsingResult); // a trick to shrink memory usage of parsingResult
	}
}