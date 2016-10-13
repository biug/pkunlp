#ifndef _POS_H
#define _POS_H
#include "token.h"

class TPOSTag {
protected:
	Token tokenizer;
	const int UNKNOWN = 0;

public:
	TPOSTag();
	~TPOSTag();

	Token & getTokenizer();
	const Token & getTokenizer() const;
	int code(const std::string & s);
};

#endif