#ifndef _TOKEN_H
#define _TOKEN_H

#include "stdafx.h"

#include <vector>
#include <string>
#include <iostream>
#include <unordered_map>

typedef std::string ttokenold;

class TokenOld {
	typedef std::vector<ttokenold>::iterator		iterator;
	typedef std::vector<ttokenold>::const_iterator	const_iterator;
protected:
	int m_nWaterMark;
	int m_nStartingToken;

	std::vector<ttokenold> m_vecKeys;
	std::unordered_map<ttokenold, int> m_mapTokens;

public:
	TokenOld(const int & nTokenStartFrom = 1) : m_nWaterMark(nTokenStartFrom), m_nStartingToken(nTokenStartFrom) {}
	TokenOld(const TokenOld & token) : m_nWaterMark(token.m_nWaterMark), m_nStartingToken(token.m_nStartingToken), m_vecKeys(token.m_vecKeys), m_mapTokens(token.m_mapTokens) {}
	~TokenOld() = default;

//	iterator begin() { return m_vecKeys.begin(); }
//	iterator end() { return m_vecKeys.end(); }
//	const_iterator begin() { return m_vecKeys.cbegin(); }
//	const_iterator end() { return m_vecKeys.cend(); }

	const int & lookup(const ttokenold & key);
	const int & find(const ttokenold & key, const int & val = 0) const;
	const ttokenold & key(const int & token) const;
	int count() const;
	void add(const ttokenold & key);
	int start() const;
	int end() const;
	void clear(const int & nTokenStartFrom = 1);

	friend std::istream & operator>>(std::istream & is, TokenOld & token) {
		int count;
		is >> count;
		ttokenold t;
		while (count--) {
			is >> t;
			token.lookup(t);
		}
		return is;
	}

	friend std::ostream & operator<<(std::ostream & os, const TokenOld & token) {
		os << token.m_vecKeys.size() << std::endl;
		for (auto && t : token.m_vecKeys) {
			os << t << std::endl;
		}
		return os;
	}
};

inline const int & TokenOld::lookup(const ttokenold & key) {
	if (m_mapTokens.find(key) == m_mapTokens.end()) {
		m_mapTokens[key] = m_nWaterMark++;
		m_vecKeys.push_back(key);
	}
	return m_mapTokens[key];
}

inline const int & TokenOld::find(const ttokenold & key, const int & val) const {
	return m_mapTokens.find(key) == m_mapTokens.end() ? val : m_mapTokens.at(key);
}

inline const ttokenold & TokenOld::key(const int & token) const {
	return m_vecKeys[token - m_nStartingToken];
}

inline int TokenOld::count() const {
	return m_vecKeys.size();
}

inline void TokenOld::add(const ttokenold & key) {
	if (m_mapTokens.find(key) == m_mapTokens.end()) {
		m_mapTokens[key] = m_nWaterMark++;
		m_vecKeys.push_back(key);
	}
}

inline int TokenOld::start() const {
	return m_nStartingToken;
}

inline int TokenOld::end() const {
	return m_nWaterMark;
}

inline void TokenOld::clear(const int & nTokenStartFrom) {
	m_nWaterMark = m_nStartingToken = nTokenStartFrom;
	m_vecKeys.clear();
	m_mapTokens.clear();
}

#endif
