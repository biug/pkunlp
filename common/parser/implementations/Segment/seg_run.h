#include "common/parser/run_base.h"
#include "seg_depparser.h"
#include <memory>

namespace Segment {
	class Run {
	public:
		Run() = default;
		~Run() = default;

		std::unique_ptr<DepParser> m_pSegmentor;
		std::string tmpParsingResult;

		void initParser(const std::string & sFeatureInput, const std::string & sFeatureOutput, const std::string & sDictPath, bool parse);
		void train(const std::string & sInputFile , int e = CP_UTF8) const;
		void parse(const std::string & sInputFile, const std::string & sOutputFile, int e = CP_UTF8) const;
		std::string parse(const std::string & sInput, int e = CP_UTF8);
		std::vector<std::pair<std::string, std::string>> parse(const std::vector<std::string> & vecInput, int e = CP_UTF8) const;

        void parseInput(const std::string &sInput, int e, char sep);

		void parseInput(const std::string &sInput, int e);
	};
}
