#include "stdafx.h"
#include "pos.h"

TPOSTagOld::TPOSTagOld() : tokenizer(1) {}

TPOSTagOld::~TPOSTagOld() = default;

int TPOSTagOld::code(const std::string & s) {
	return tokenizer.lookup(s);
}

TokenOld & TPOSTagOld::getTokenizer() {
	return tokenizer;
}

const TokenOld & TPOSTagOld::getTokenizer() const {
	return tokenizer;
}