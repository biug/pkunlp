#include "common/parser/run_base.h"
#include "postag_depparser.h"
#include <memory>

#pragma once

namespace POSTagging {

    struct WordWithTag {
        const char* word;
        const char* tag;

        WordWithTag(const char* word, const char* tag);
    };

	struct ManagedWordWithTag {
		std::string word;
		std::string tag;

        ManagedWordWithTag(const std::string word, const std::string tag);
        WordWithTag c_str();
	};

	struct TaggingResult {
		WordWithTag* words;
		int length;

		TaggingResult(WordWithTag *words, int length);
	};

	class Run {
	public:
		Run() = default;
		~Run() = default;

		std::unique_ptr<DepParser> m_pPOSTagger;
		std::vector<ManagedWordWithTag> parsingResult;
		std::vector<WordWithTag> storedResult;

		void initParser(const std::string & sFeatureInput, const std::string & sFeatureOutput, const std::string & sDictPath, bool parse);
		void train(const std::string & sInputFile, int e = CP_UTF8) const;
		void parse(const std::string & sInputFile, const std::string & sOutputFile, int e = CP_UTF8) const;
		std::string parse(const std::string & sInput, int e = CP_UTF8) const;
		std::vector<std::pair<std::string, std::string>> parse(const std::vector<std::string> & vecInput, int e = CP_UTF8) const;

		TaggingResult parseToResult(char **vecInput, int length, int e);
	};
}
