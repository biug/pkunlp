#ifndef _POS_H
#define _POS_H
#include "token.h"

class TPOSTagOld {
protected:
	TokenOld tokenizer;
	const int UNKNOWN = 0;

public:
	TPOSTagOld();
	~TPOSTagOld();

	TokenOld & getTokenizer();
	const TokenOld & getTokenizer() const;
	int code(const std::string & s);
};

#endif