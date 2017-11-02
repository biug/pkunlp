#include "stdafx.h"
#include "word.h"

TWordOld::TWordOld() : tokenizer(1) {}

TWordOld::~TWordOld() = default;

int TWordOld::code(const std::string & s) {
	return tokenizer.lookup(s);
}

TokenOld & TWordOld::getTokenizer() {
	return TWordOld::tokenizer;
}

const TokenOld & TWordOld::getTokenizer() const {
	return TWordOld::tokenizer;
}