//
// Copyright (C) 2010, OMBT LLC and Mike A. Rumore
// All rights reserved.
// Contact: Mike A. Rumore, (mike.a.rumore@gmail.com)
//
#ifndef __OMBT_TRIE_H
#define __OMBT_TRIE_H
// trie class definitions

// headers
#include <stdlib.h>
#include <iostream>

// local headers
#include "system/Returns.h"
#include "system/Debug.h"
#include "gstring/gstring.h"
#include "adt/Queue_List.h"

namespace ombt {

// forward declarations
template <class DataType> class TrieNode;
template <class DataType> class Trie;
template <class DataType> class TrieIterator;

// forward declarations of friend functions 
template <class DataType> 
std::ostream &
operator<<(std::ostream &, TrieNode<DataType> &);

template <class DataType> 
std::ostream &
operator<<(std::ostream &, Trie<DataType> &);

// trie node class
template <class DataType> class TrieNode {
public:
	// output
	friend std::ostream &operator<< <>(std::ostream &os, TrieNode<DataType> &);

protected:
        // friend classes 
        friend class Trie<DataType>;
	friend class TrieIterator<DataType>;

        // constructors and destructor
        TrieNode(const DataType &);
        TrieNode(const TrieNode<DataType> &);
        ~TrieNode();

	// assignment
        TrieNode<DataType> &operator=(const TrieNode<DataType> &);

        // internal data
	bool isaword;
        DataType data;
        TrieNode<DataType> *sibling;
        TrieNode<DataType> *children;
};

// trie class
template <class DataType> class Trie 
{
public:
	// friend classes 
	friend class TrieIterator<DataType>;

        // constructors and destructor
        Trie();
        Trie(const GString<DataType> &);
        Trie(const Trie<DataType> &);
        ~Trie();

        // assignment
        Trie<DataType> &operator=(const Trie<DataType> &);
        Trie<DataType> &operator=(const GString<DataType> &);

        // trie operations
        int insert(const GString<DataType> &);
        int remove(const GString<DataType> &);
        bool includes(const GString<DataType> &) const;
	int isEmpty() const;
        void clear();

        // miscellaneous
        std::ostream &dump(std::ostream &) const;

	// output
	friend std::ostream &operator<< <>(std::ostream &, Trie<DataType> &);

protected:
	// utility functions
        void dump(const TrieNode<DataType> *, GString<DataType>, 
			std::ostream &) const;
	int insert(TrieNode<DataType> *&, const GString<DataType> &, int);
	int createpath(TrieNode<DataType> *&, const GString<DataType> &, int);
	TrieNode<DataType> *copy() const;
	TrieNode<DataType> *copy(const TrieNode<DataType> *) const;
        void clear(TrieNode<DataType> *&);
        bool includes(const TrieNode<DataType> *, const GString<DataType> &,
			int) const;
	int remove(TrieNode<DataType> *&, const GString<DataType> &, int);

protected:
        // internal data
        TrieNode<DataType> *root;
};

// depth-first trie iterator
template <class DataType> class TrieIterator {
public:
        // constructors and destructor
        TrieIterator(const Trie<DataType> &);
        TrieIterator(const TrieIterator<DataType> &);
        ~TrieIterator();

	// reset iterator to start
	void reset();

	// check if at end of list
	int done() const;

        // return data 
        GString<DataType> operator()();

	// advance iterator to next link
	int operator++(int);

private:
	// not allowed
        TrieIterator();
        TrieIterator &operator=(const TrieIterator<DataType> &);

	// utility functions
	void reset(Queue_List<GString<DataType> > &,
		const TrieNode<DataType> *, GString<DataType>);

protected:
        // internal data
	const Trie<DataType> &trie;
	Queue_List<GString<DataType> > queue;
};

#include "gstring/trie.i"

}

#endif

