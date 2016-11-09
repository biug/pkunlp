#ifndef _RUN_H
#define _RUN_H

#include "stdafx.h"

#include <string>

#include "depparser_base.h"

class RunBase {
public:
	RunBase() = default;
	virtual ~RunBase() {};

	virtual void train(const std::string & sInputFile, const std::string & sFeatureInput, const std::string & sFeatureOutput) = 0;
	virtual void parse(const std::string & sInputFile, const std::string & sOutputFile, const std::string & sFeatureFile) = 0;
	virtual void goldtest(const std::string & sInputFile, const std::string & sFeatureInput) = 0;
};

#endif
