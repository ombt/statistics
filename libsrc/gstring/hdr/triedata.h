//
// Copyright (C) 2010, OMBT LLC and Mike A. Rumore
// All rights reserved.
// Contact: Mike A. Rumore, (mike.a.rumore@gmail.com)
//
#ifndef __OMBT_TRIE_DATA_H
#define __OMBT_TRIE_DATA_H

// trie with data payload class definitions

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
template <class CharacterType, class PayloadType> class TrieDataNode;
template <class CharacterType, class PayloadType> class TrieData;

// forward declarations of friend functions 
template <class CharacterType, class PayloadType> 
std::ostream &
operator<<(std::ostream &, TrieDataNode<CharacterType, PayloadType> &);

template <class CharacterType, class PayloadType> 
std::ostream &
operator<<(std::ostream &, TrieData<CharacterType, PayloadType> &);

// trie node class
template <class CharacterType, class PayloadType> class TrieDataNode {
public:
	// output
	friend std::ostream &operator<< <>(std::ostream &os, TrieDataNode<CharacterType, PayloadType> &);

protected:
        // friend classes 
        friend class TrieData<CharacterType, PayloadType>;

        // constructors and destructor
        TrieDataNode(const CharacterType &);
        TrieDataNode(const TrieDataNode<CharacterType, PayloadType> &);
        ~TrieDataNode();

	// assignment
        TrieDataNode<CharacterType, PayloadType> &operator=(const TrieDataNode<CharacterType, PayloadType> &);

        // internal data
	bool isaword;
        CharacterType data;
        PayloadType payload;
        TrieDataNode<CharacterType, PayloadType> *sibling;
        TrieDataNode<CharacterType, PayloadType> *children;
};

// trie class
template <class CharacterType, class PayloadType> class TrieData 
{
public:
        // constructors and destructor
        TrieData();
        TrieData(const GString<CharacterType> &);
        TrieData(const TrieData<CharacterType, PayloadType> &);
        ~TrieData();

        // assignment
        TrieData<CharacterType, PayloadType> &operator=(const TrieData<CharacterType, PayloadType> &);
        TrieData<CharacterType, PayloadType> &operator=(const GString<CharacterType> &);

        // trie operations
        int insert(const GString<CharacterType> &, const PayloadType &payload);
        int remove(const GString<CharacterType> &, bool recursive = false);
        bool includes(const GString<CharacterType> &) const;
        int read(const GString<CharacterType> &, PayloadType &payload) const;
        int update(const GString<CharacterType> &, const PayloadType &payload);
	int getChildren(const GString<CharacterType> &, std::list<CharacterType> &) const;
	int isEmpty() const;
        void clear();

        // miscellaneous
        std::ostream &dump(std::ostream &) const;

	// output
	friend std::ostream &operator<< <>(std::ostream &, TrieData<CharacterType, PayloadType> &);

protected:
	// utility functions
        void dump(const TrieDataNode<CharacterType, PayloadType> *, GString<CharacterType>, std::ostream &) const;
	int insert(TrieDataNode<CharacterType, PayloadType> *&, const GString<CharacterType> &, int, const PayloadType &);
	int createpath(TrieDataNode<CharacterType, PayloadType> *&, const GString<CharacterType> &, int, const PayloadType &);
	TrieDataNode<CharacterType, PayloadType> *copy() const;
	TrieDataNode<CharacterType, PayloadType> *copy(const TrieDataNode<CharacterType, PayloadType> *) const;
        void clear(TrieDataNode<CharacterType, PayloadType> *&);
        bool includes(const TrieDataNode<CharacterType, PayloadType> *, const GString<CharacterType> &, int) const;
        int read(const TrieDataNode<CharacterType, PayloadType> *, const GString<CharacterType> &, int, PayloadType &) const;
        int getChildren(const TrieDataNode<CharacterType, PayloadType> *, const GString<CharacterType> &, int, std::list<CharacterType> &) const;
        int update(TrieDataNode<CharacterType, PayloadType> *, const GString<CharacterType> &, int, const PayloadType &);
	int remove(TrieDataNode<CharacterType, PayloadType> *&, const GString<CharacterType> &, int, bool recursive);

protected:
        // internal data
        TrieDataNode<CharacterType, PayloadType> *root;
};

#include "gstring/triedata.i"

}

#endif

