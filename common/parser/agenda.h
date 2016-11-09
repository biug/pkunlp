#include "stdafx.h"

#include <utility>
#include <iterator>

#ifndef _AGENDA_H
#define _AGENDA_H

template<typename T>
class BeamIterator {
public:
	typedef BeamIterator<T>	self;

	typedef T				value_type;
	typedef T*				pointer;
	typedef T&				reference;

public:
	T* node;

public:
	BeamIterator() : node(nullptr) {};
	BeamIterator(T * x) : node(x) {}
	BeamIterator(const self & b) : node(b.node) {}

	bool operator==(const self & x) const { return node == x.node; }
	bool operator!=(const self & x) const { return node != x.node; }

	reference operator*() { return *node; }
	pointer operator->() { return node; }

	reference operator*() const { return *node; }
	pointer operator->() const { return node; }

	self & operator++() {
		++node;
		return *this;
	}
	self & operator++(int) {
		self tmp = *this;
		++*this;
		return tmp;
	}
	self & operator--() {
		--node;
		return *this;
	}
	self & operator--(int) {
		self tmp = *this;
		++*this;
		return tmp;
	}
};

template<typename KEY_TYPE, int SIZE>
class AgendaBeam {
public:
	typedef BeamIterator<KEY_TYPE *>				iterator;
	typedef BeamIterator<KEY_TYPE * const>			const_iterator;

private:
	int m_nBeamSize;
	bool m_bItemSorted;
	KEY_TYPE * m_lBeam[SIZE];
	KEY_TYPE m_lEntity[SIZE];

	void pop_heap();
	void push_heap(int base);

	AgendaBeam<KEY_TYPE, SIZE> & operator=(const AgendaBeam<KEY_TYPE, SIZE> & ab) {
		m_nBeamSize = ab.m_nBeamSize;
		m_bItemSorted = ab.m_bItemSorted;
		for (int i = 0; i < SIZE; ++i) {
			m_lEntity[i] = ab.m_lEntity[i];
			m_lBeam[i] = &m_lEntity[ab.m_lBeam[i] - ab.m_lEntity];
		}
		return *this;
	}

	AgendaBeam<KEY_TYPE, SIZE> & operator=(AgendaBeam<KEY_TYPE, SIZE> && ab) {
		m_nBeamSize = ab.m_nBeamSize;
		m_bItemSorted = ab.m_bItemSorted;
		for (int i = 0; i < SIZE; ++i) {
			m_lEntity[i] = ab.m_lEntity[i];
			m_lBeam[i] = &m_lEntity[ab.m_lBeam[i] - ab.m_lEntity];
			ab.m_lBeam[i] = nullptr;
		}
		return *this;
	}

public:

	AgendaBeam() : m_nBeamSize(0), m_bItemSorted(false) {
		for (int i = 0; i < SIZE; ++i) {
			m_lBeam[i] = &m_lEntity[i];
		}
	}
	AgendaBeam(const AgendaBeam<KEY_TYPE, SIZE> & ab) : m_nBeamSize(ab.m_nBeamSize), m_bItemSorted(ab.m_bItemSorted) {
		for (int i = 0; i < SIZE; ++i) {
			m_lEntity[i] = ab.m_lEntity[i];
			m_lBeam[i] = &m_lEntity[ab.m_lBeam[i] - ab.m_lEntity];
		}
	}
	AgendaBeam(AgendaBeam<KEY_TYPE, SIZE> && ab) : m_nBeamSize(ab.m_nBeamSize), m_bItemSorted(ab.m_bItemSorted) {
		for (int i = 0; i < SIZE; ++i) {
			m_lEntity[i] = ab.m_lEntity[i];
			m_lBeam[i] = &m_lEntity[ab.m_lBeam[i] - ab.m_lEntity];
			ab.m_lBeam[i] = nullptr;
		}
	}
	~AgendaBeam() = default;

	const int & size() const;
	void clear();

	void insertItem(const KEY_TYPE & item);
	void sortItems();
	const KEY_TYPE & bestItem(const int & index = 0);
	const KEY_TYPE & bestUnsortItem(const int & index = 0);

	iterator begin() { return &m_lBeam[0]; }
	iterator end() { return &m_lBeam[m_nBeamSize]; }
	const_iterator begin() const { return &m_lBeam[0]; }
	const_iterator end() const { return &m_lBeam[m_nBeamSize]; }

};

template<typename KEY_TYPE, int SIZE>
void AgendaBeam<KEY_TYPE, SIZE>::push_heap(int base) {
	while (base > 0) {
		int next_base = (base - 1) >> 1;
		if (*m_lBeam[next_base] > *m_lBeam[base]) {
			std::swap(m_lBeam[next_base], m_lBeam[base]);
			base = next_base;
		}
		else {
			break;
		}
	}
}

template<typename KEY_TYPE, int SIZE>
void AgendaBeam<KEY_TYPE, SIZE>::pop_heap() {
	if (m_nBeamSize <= 0) {
		return;
	}
	std::swap(m_lBeam[0], m_lBeam[--m_nBeamSize]);
	KEY_TYPE * smallest = m_lBeam[0];
	int index = 0, child = 1;
	while (child < m_nBeamSize) {
		if (child + 1 < m_nBeamSize && *m_lBeam[child + 1] <= *m_lBeam[child]) {
			++child;
		}
		m_lBeam[index] = m_lBeam[child];
		index = child;
		child = (child << 1) + 1;
	}
	m_lBeam[index] = smallest;
	push_heap(index);
}

template<typename KEY_TYPE, int SIZE>
inline void AgendaBeam<KEY_TYPE, SIZE>::clear() {
	m_nBeamSize = 0;
	m_bItemSorted = false;
}

template<typename KEY_TYPE, int SIZE>
inline const int & AgendaBeam<KEY_TYPE, SIZE>::size() const {
	 return m_nBeamSize;
}

template<typename KEY_TYPE, int SIZE>
void AgendaBeam<KEY_TYPE, SIZE>::insertItem(const KEY_TYPE & item) {
	if (m_nBeamSize == SIZE) {
		if (item > *m_lBeam[0]) {
			pop_heap();
		}
		else {
			return;
		}
	}
	*m_lBeam[m_nBeamSize] = item;
	push_heap(m_nBeamSize++);
}

template<typename KEY_TYPE, int SIZE>
void AgendaBeam<KEY_TYPE, SIZE>::sortItems() {
	if (m_bItemSorted) {
		return;
	}
	int size = m_nBeamSize;
	while (m_nBeamSize > 0) {
		pop_heap();
	}
	m_nBeamSize = size;
	m_bItemSorted = true;
}

template<typename KEY_TYPE, int SIZE>
const KEY_TYPE & AgendaBeam<KEY_TYPE, SIZE>::bestItem(const int & index) {
	if (!m_bItemSorted) {
		sortItems();
	}
	return *m_lBeam[0];
}

template<typename KEY_TYPE, int SIZE>
const KEY_TYPE & AgendaBeam<KEY_TYPE, SIZE>::bestUnsortItem(const int & index) {
	int best_index = 0;
	for (int i = 1; i < m_nBeamSize; ++i) {
		if (*m_lBeam[best_index] < *m_lBeam[i]) {
			best_index = i;
		}
	}
	return *m_lBeam[best_index];
}

#endif
