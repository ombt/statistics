//
// Copyright (C) 2010, OMBT LLC and Mike A. Rumore
// All rights reserved.
// Contact: Mike A. Rumore, (mike.a.rumore@gmail.com)
//
#ifndef __ONBT_TRIE_INDEX_H
#define __ONBT_TRIE_INDEX_H
// trie class definitions

// headers
#include <stdlib.h>
#include <assert.h>
#include <iostream>
#include <list>

// local headers
#include "system/Returns.h"
#include "system/Debug.h"
#include "gstring/gstring.h"

namespace ombt {

// forward declarations
template <class CharacterType> class TrieIndexNode;
template <class CharacterType> class TrieIndex;

// forward declarations of friend functions 
template <class CharacterType> 
std::ostream &
operator<<(std::ostream &, TrieIndexNode<CharacterType> &);

template <class CharacterType> 
std::ostream &
operator<<(std::ostream &, TrieIndex<CharacterType> &);

// trie node class
template <class CharacterType> class TrieIndexNode {
public:
	// output
	friend std::ostream &operator<< <>(std::ostream &os, TrieIndexNode<CharacterType> &);

protected:
        // friend classes 
        friend class TrieIndex<CharacterType>;

        // constructors and destructor
        TrieIndexNode(const CharacterType &);
        TrieIndexNode(const TrieIndexNode<CharacterType> &);
        ~TrieIndexNode();

	// assignment
        TrieIndexNode<CharacterType> &operator=(const TrieIndexNode<CharacterType> &);

        // internal data
	bool isaword;
        CharacterType data;
        TrieIndexNode<CharacterType> *sibling;
        TrieIndexNode<CharacterType> *children;
};

// trie class
template <class CharacterType> class TrieIndex 
{
public:
        // constructors and destructor
        TrieIndex();
        TrieIndex(const GString<CharacterType> &);
        TrieIndex(const TrieIndex<CharacterType> &);
        ~TrieIndex();

        // assignment
        TrieIndex<CharacterType> &operator=(const TrieIndex<CharacterType> &);
        TrieIndex<CharacterType> &operator=(const GString<CharacterType> &);

        // trie operations
        int insert(const GString<CharacterType> &);
        int remove(const GString<CharacterType> &, bool recursive = false);
        bool includes(const GString<CharacterType> &) const;
	int getChildren(const GString<CharacterType> &, std::list<CharacterType> &) const;
	int isEmpty() const;
        void clear();

        // miscellaneous
        std::ostream &dump(std::ostream &) const;

	// output
	friend std::ostream &operator<< <>(std::ostream &, TrieIndex<CharacterType> &);

protected:
	// utility functions
        void dump(const TrieIndexNode<CharacterType> *, GString<CharacterType>, std::ostream &) const;
	int insert(TrieIndexNode<CharacterType> *&, const GString<CharacterType> &, int);
	int createpath(TrieIndexNode<CharacterType> *&, const GString<CharacterType> &, int);
	TrieIndexNode<CharacterType> *copy() const;
	TrieIndexNode<CharacterType> *copy(const TrieIndexNode<CharacterType> *) const;
        void clear(TrieIndexNode<CharacterType> *&);
        bool includes(const TrieIndexNode<CharacterType> *, const GString<CharacterType> &, int) const;
        int getChildren(const TrieIndexNode<CharacterType> *, const GString<CharacterType> &, int, std::list<CharacterType> &) const;
	int remove(TrieIndexNode<CharacterType> *&, const GString<CharacterType> &, int, bool recursive);

protected:
        // internal data
        TrieIndexNode<CharacterType> *root;
};

#include "gstring/trieindex.i"

}

#endif

