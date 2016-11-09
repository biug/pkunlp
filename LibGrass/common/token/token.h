#ifndef _TOKEN_H
#define _TOKEN_H

#include "stdafx.h"

#include <vector>
#include <string>
#include <iostream>
#include <unordered_map>

typedef std::string ttoken;

class Token {
	typedef std::vector<ttoken>::iterator		iterator;
	typedef std::vector<ttoken>::const_iterator	const_iterator;
protected:
	int m_nWaterMark;
	int m_nStartingToken;

	std::vector<ttoken> m_vecKeys;
	std::unordered_map<ttoken, int> m_mapTokens;

public:
	Token(const int & nTokenStartFrom = 1) : m_nWaterMark(nTokenStartFrom), m_nStartingToken(nTokenStartFrom) {}
	Token(const Token & token) : m_nWaterMark(token.m_nWaterMark), m_nStartingToken(token.m_nStartingToken), m_vecKeys(token.m_vecKeys), m_mapTokens(token.m_mapTokens) {}
	~Token() = default;

//	iterator begin() { return m_vecKeys.begin(); }
//	iterator end() { return m_vecKeys.end(); }
//	const_iterator begin() { return m_vecKeys.cbegin(); }
//	const_iterator end() { return m_vecKeys.cend(); }

	const int & lookup(const ttoken & key);
	const int & find(const ttoken & key, const int & val = 0) const;
	const ttoken & key(const int & token) const;
	int count() const;
	void add(const ttoken & key);
	int start() const;
	int end() const;
	void clear(const int & nTokenStartFrom = 1);

	friend std::istream & operator>>(std::istream & is, Token & token) {
		int count;
		is >> count;
		ttoken t;
		while (count--) {
			is >> t;
			token.lookup(t);
		}
		return is;
	}

	friend std::ostream & operator<<(std::ostream & os, const Token & token) {
		os << token.m_vecKeys.size() << std::endl;
		for (auto && t : token.m_vecKeys) {
			os << t << std::endl;
		}
		return os;
	}
};

inline const int & Token::lookup(const ttoken & key) {
	if (m_mapTokens.find(key) == m_mapTokens.end()) {
		m_mapTokens[key] = m_nWaterMark++;
		m_vecKeys.push_back(key);
	}
	return m_mapTokens[key];
}

inline const int & Token::find(const ttoken & key, const int & val) const {
	return m_mapTokens.find(key) == m_mapTokens.end() ? val : m_mapTokens.at(key);
}

inline const ttoken & Token::key(const int & token) const {
	return m_vecKeys[token - m_nStartingToken];
}

inline int Token::count() const {
	return m_vecKeys.size();
}

inline void Token::add(const ttoken & key) {
	if (m_mapTokens.find(key) == m_mapTokens.end()) {
		m_mapTokens[key] = m_nWaterMark++;
		m_vecKeys.push_back(key);
	}
}

inline int Token::start() const {
	return m_nStartingToken;
}

inline int Token::end() const {
	return m_nWaterMark;
}

inline void Token::clear(const int & nTokenStartFrom) {
	m_nWaterMark = m_nStartingToken = nTokenStartFrom;
	m_vecKeys.clear();
	m_mapTokens.clear();
}

#endif
