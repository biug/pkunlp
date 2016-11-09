#ifndef _TAGSET_H
#define _TAGSET_H

#include "stdafx.h"

#include <iostream>

template <int SIZE>
class TagSetN {
private:
	unsigned int m_lBits[SIZE];
public:
	TagSetN() {
		memset(m_lBits, 0, sizeof(m_lBits));
	}
	TagSetN(const TagSetN<SIZE> & tagset) {
		memcpy(m_lBits, tagset.m_lBits, sizeof(m_lBits));
	}
	~TagSetN() = default;

	void add(const int & t) {
		m_lBits[t >> 5] |= (unsigned int)(1 << (t - ((t >> 5) << 5)));
	}
	void remove(const int & t) {
		m_lBits[t >> 5] &= ~(unsigned int)(1 << (t - ((t >> 5) << 5)));
	}
	void clear() {
		memset(m_lBits, 0, sizeof(m_lBits));
	}
	bool contain(const int & t) {
		return (m_lBits[t >> 5] & (unsigned int)(1 << (t - ((t >> 5) << 5)))) != 0;
	}
	const unsigned int & bits(const int & i) const {
		return m_lBits[i];
	}
	bool operator==(const TagSetN<SIZE> & tagset) const {
		for (int i = 0; i < SIZE; ++i) {
			if (m_lBits[i] != tagset.m_lBits[i]) {
				return false;
			}
		}
		return true;
	}
	bool operator!=(const TagSetN<SIZE> & tagset) const {
		return !(*this == tagset);
	}
	TagSetN<SIZE> & operator=(const TagSetN<SIZE> & tagset) {
		memcpy(m_lBits, tagset.m_lBits, sizeof(m_lBits));
		return *this;
	}

	friend std::istream & operator>>(std::istream & is, TagSetN<SIZE> & tagset) {
		for (int i = SIZE - 1; i >= 0; --i) {
			is >> tagset.m_lBits[i];
		}
		return is;
	}

	friend std::ostream & operator<<(std::ostream & os, const TagSetN<SIZE> & tagset) {
		os << tagset.m_lBits[SIZE - 1];
		for (int i = SIZE - 2; i >= 0; --i) {
			os << " " << tagset.m_lBits[i];
		}
		return os;
	}
};

#endif
