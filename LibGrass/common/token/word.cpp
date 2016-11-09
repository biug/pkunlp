#include "stdafx.h"
#include "word.h"

TWord::TWord() : tokenizer(1) {}

TWord::~TWord() = default;

int TWord::code(const std::string & s) {
	return tokenizer.lookup(s);
}

Token & TWord::getTokenizer() {
	return TWord::tokenizer;
}

const Token & TWord::getTokenizer() const {
	return TWord::tokenizer;
}