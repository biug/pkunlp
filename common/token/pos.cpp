#include "stdafx.h"
#include "pos.h"

TPOSTag::TPOSTag() : tokenizer(1) {}

TPOSTag::~TPOSTag() = default;

int TPOSTag::code(const std::string & s) {
	return tokenizer.lookup(s);
}

Token & TPOSTag::getTokenizer() {
	return tokenizer;
}

const Token & TPOSTag::getTokenizer() const {
	return tokenizer;
}