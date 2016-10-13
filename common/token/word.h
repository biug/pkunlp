#ifndef _WORD_H
#define _WORD_H
#include "token.h"

class TWord {
protected:
	Token tokenizer;
	const int UNKNOWN = 0;

public:
	TWord();
	~TWord();

	Token & getTokenizer();
	const Token & getTokenizer() const;
	int code(const std::string & s);
};

#endif