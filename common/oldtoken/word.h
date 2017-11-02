#ifndef _WORD_H
#define _WORD_H
#include "token.h"

class TWordOld {
protected:
	TokenOld tokenizer;
	const int UNKNOWN = 0;

public:
	TWordOld();
	~TWordOld();

	TokenOld & getTokenizer();
	const TokenOld & getTokenizer() const;
	int code(const std::string & s);
};

#endif