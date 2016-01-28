//
// Copyright (C) 2010, OMBT LLC and Mike A. Rumore
// All rights reserved.
// Contact: Mike A. Rumore, (mike.a.rumore@gmail.com)
//
// member functions for a trie

// trie node constructors and destructor
template <class DataType>
TrieNode<DataType>::TrieNode(const DataType &d):
	isaword(false), data(d), sibling(NULL), children(NULL)
{
	// do nothing
}

template <class DataType>
TrieNode<DataType>::TrieNode(const TrieNode<DataType> &tn):
	isaword(tn.isaword), data(tn.data), sibling(NULL), children(NULL)
{
	// do nothing
}

template <class DataType>
TrieNode<DataType>::~TrieNode()
{
	isaword = false;
	sibling = NULL;
	children = NULL;
}

// assignment
template <class DataType>
TrieNode<DataType> &
TrieNode<DataType>::operator=(const TrieNode<DataType> &tn)
{
	if (this != &tn)
	{
		sibling = NULL;
		children = NULL;
		data = tn.data;
		isaword = tn.isaword;
	}
	return(*this);
}

// output operator
template <class DataType>
std::ostream &
operator<<(std::ostream &os, TrieNode<DataType> &tn)
{
	os << tn.data;
	return(os);
}

// trie constructors and destructor
template <class DataType>
Trie<DataType>::Trie():
	root(NULL)
{
	// do nothing
}

template <class DataType>
Trie<DataType>::Trie(const Trie<DataType> &t):
	root(NULL)
{
	root = t.copy();
}

template <class DataType>
Trie<DataType>::~Trie()
{
	clear();
}

// assignment
template <class DataType>
Trie<DataType> &
Trie<DataType>::operator=(const Trie<DataType> &t)
{
	if (this != &t)
	{
		clear();
		root = t.copy();
	}
	return(*this);
}

// binary tree operations
template <class DataType>
int
Trie<DataType>::insert(const GString<DataType> &gs)
{
	// start scanning sibling list, look for first letter.
	TrieNode<DataType> *proot = root;
	TrieNode<DataType> *pprev = NULL;
	int idx = 0;
	for ( ; proot != NULL; pprev = proot, proot = proot->sibling)
	{
		// does data match first character in the string
		if (proot->data < gs[idx])
		{
			continue;
		}
		else if (proot->data == gs[idx])
		{
			// we have a match for the current letter, 
			// check if the entire word was scanned
			if ((idx+1) >= gs.gstrlen())
			{
				// the new word is a prefix of an
				// existing word; set the is-a-word 
				// bit to true
				proot->isaword = true;
				return(OK);
			}
			else
			{
				// continue the search, check children
				return(insert(proot->children, gs, idx+1));
			}
		}
		else
		{
			break;
		}
	}

	// not found, create a new path to the new word
	TrieNode<DataType> *pnew = NULL;
	if (createpath(pnew, gs, idx) != OK)
		return(NOTOK);

	// store new sibling at the root
	if (pprev == NULL)
	{
		pnew->sibling = root;
		root = pnew;
	}
	else
	{
		pnew->sibling = pprev->sibling;
		pprev->sibling = pnew;
	}
	return(OK);
}

template <class DataType>
int
Trie<DataType>::insert(TrieNode<DataType> *&pchild, 
	const GString<DataType> &gs, int idx)
{
	// look for a match
	if (pchild == NULL)
	{
		// word was not found, add new path
		return(createpath(pchild, gs, idx));
	}

	// start scanning sibling list
	TrieNode<DataType> *pch = pchild;
	TrieNode<DataType> *pprev = NULL;
	for ( ; pch != NULL; pprev = pch, pch = pch->sibling)
	{
		// does data match current character in the string
		if (pch->data < gs[idx])
		{
			continue;
		}
		else if (pch->data == gs[idx])
		{
			// we have a match for the current letter, 
			// check if the entire word was scanned
			if ((idx+1) >= gs.gstrlen())
			{
				// the new word is a prefix of an
				// existing word; set the is-a-word 
				// bit to true
				pch->isaword = true;
				return(OK);
			}
			else
			{
				// continue the search, check children
				return(insert(pch->children, gs, idx+1));
			}
		}
		else
		{
			break;
		}
	}

	// not found, create a new path to the new word
	TrieNode<DataType> *pnew = NULL;
	if (createpath(pnew, gs, idx) != OK)
		return(NOTOK);

	// store new sibling at the current child
	if (pprev == NULL)
	{
		pnew->sibling = pchild;
		pchild = pnew;
	}
	else
	{
		pnew->sibling = pprev->sibling;
		pprev->sibling = pnew;
	}
	return(OK);
}

template <class DataType>
int
Trie<DataType>::createpath(TrieNode<DataType> *&pnode, 
	const GString<DataType> &gs, int idx)
{
	// must not be at the end
	MustBeTrue(idx < gs.gstrlen());

	// allocate a new node
	pnode = new TrieNode<DataType>(gs[idx]);
	MustBeTrue(pnode != NULL)

	// continue ?
	if ((idx+1) >= gs.gstrlen())
	{
		// at the end of a word
		pnode->isaword = true;
		return(OK);
	}
	else
	{
		// do the next node
		return(createpath(pnode->children, gs, idx+1));
	}
}

template <class DataType>
int
Trie<DataType>::remove(const GString<DataType> &gs)
{
	// start scanning sibling list
	TrieNode<DataType> *proot = root;
	TrieNode<DataType> *pprev = NULL;
	for (int idx = 0; proot != NULL; pprev = proot, proot = proot->sibling)
	{
		// does data match first character in the string
		if (proot->data < gs[idx])
		{
			continue;
		}
		else if (proot->data == gs[idx])
		{
			// we have a match for the current letter, 
			// check if the entire word was scanned
			if ((idx+1) >= gs.gstrlen() && proot->isaword)
			{
				// word was found. check if this node
				// has any children
				if (proot->children != NULL)
				{
					// this node has children,
					// just turn off the word bit.
					proot->isaword = false;
				}
				else if (pprev == NULL)
				{
					// node has no children and it is
					// the first node in sibling list
					pprev = root->sibling;
					delete root;
					root = pprev;
				}
				else
				{
					// node has no children,
					// remove internal or end node
					pprev->sibling = proot->sibling;
					delete proot;
				}
				return(OK);
			}
			else
			{
				// continue the search, check children
				return(remove(proot->children, gs, idx+1));
			}
		}
		else
		{
			// not found
			break;
		}
	}

	// not found
	return(NOMATCH);
}

template <class DataType>
bool
Trie<DataType>::includes(const GString<DataType> &gs) const
{
	// start scanning sibling list
	TrieNode<DataType> *proot = root;
	for (int idx = 0; proot != NULL; proot = proot->sibling)
	{
		// does data match first character in the string
		if (proot->data < gs[idx])
		{
			continue;
		}
		else if (proot->data == gs[idx])
		{
			// we have a match for the current letter, 
			// check if the entire word was scanned
			if ((idx+1) >= gs.gstrlen() && proot->isaword)
			{
				// word was found
				return(true);
			}
			else
			{
				// continue the search, check children
				return(includes(proot->children, gs, idx+1));
			}
		}
		else
		{
			// not found
			break;
		}
	}

	// not found
	return(false);
}

template <class DataType>
int
Trie<DataType>::isEmpty() const
{
	return(root == NULL);
}

template <class DataType>
void
Trie<DataType>::clear()
{
	// clear trie
	clear(root);
	return;
}

// print binary tree data
template <class DataType>
std::ostream &
Trie<DataType>::dump(std::ostream &os) const
{
	GString<DataType> gs;
	dump(root, gs, os);
	return(os);
}

// output function
template <class DataType>
std::ostream &
operator<<(std::ostream &os, Trie<DataType> &t)
{
	t.dump(os);
	return(os);
}


// utility functions
template <class DataType>
TrieNode<DataType> *
Trie<DataType>::copy() const
{
	return(copy(root));
}

template <class DataType>
TrieNode<DataType> *
Trie<DataType>::copy(const TrieNode<DataType> *pnode) const
{
	TrieNode<DataType> *pnew = NULL;
	if (pnode != NULL)
	{
		pnew = new TrieNode<DataType>(*pnode);
		MustBeTrue(pnew != NULL);
		pnew->sibling = copy(pnode->sibling);
		pnew->children = copy(pnode->children);
	}
	return(pnew);
}

template <class DataType>
void
Trie<DataType>::clear(TrieNode<DataType> *&pnode)
{
	if (pnode != NULL)
	{
		clear(pnode->sibling);
		clear(pnode->children);
		delete pnode;
		pnode = NULL;
	}
	return;
}

template <class DataType>
bool
Trie<DataType>::includes( const TrieNode<DataType> *pnode, 
	const GString<DataType> &gs, int idx) const
{
	// look for a match
	if (pnode == NULL || (idx >= gs.gstrlen()))
	{
		// word was not found
		return(false);
	}

	// start scanning sibling list
	for ( ; pnode != NULL; pnode = pnode->sibling)
	{
		// does data match current character in the string
		if (pnode->data < gs[idx])
		{
			continue;
		}
		else if (pnode->data == gs[idx])
		{
			// we have a match for the current letter, 
			// check if the entire word was scanned
			if ((idx+1) == gs.gstrlen() && pnode->isaword)
			{
				// word was found
				return(true);
			}
			else
			{
				// continue the search, check children
				return(includes(pnode->children, gs, idx+1));
			}
		}
		else
		{
			// not found
			break;
		}
	}

	// not found
	return(false);
}

template <class DataType>
int
Trie<DataType>::remove(TrieNode<DataType> *&pchild, 
	const GString<DataType> &gs, int idx)
{
	// look for a match
	if (pchild == NULL || (idx >= gs.gstrlen()))
	{
		// word was not found
		return(NOMATCH);
	}

	// start scanning sibling list
	TrieNode<DataType> *pnode = pchild;
	TrieNode<DataType> *pprev = NULL;
	for ( ; pnode != NULL; pprev = pnode, pnode = pnode->sibling)
	{
		// does data match first character in the string
		if (pnode->data < gs[idx])
		{
			continue;
		}
		else if (pnode->data == gs[idx])
		{
			// we have a match for the current letter, 
			// check if the entire word was scanned
			if ((idx+1) >= gs.gstrlen() && pnode->isaword)
			{
				// word was found. check if this node
				// has any children
				if (pnode->children != NULL)
				{
					// this node has children,
					// just turn off the word bit.
					pnode->isaword = false;
				}
				else if (pprev == NULL)
				{
					// node has no children and it is
					// the first node in sibling list
					pprev = pchild->sibling;
					delete pchild;
					pchild = pprev;
				}
				else
				{
					// node has no children,
					// remove internal or end node
					pprev->sibling = pnode->sibling;
					delete pnode;
				}
				return(OK);
			}
			else
			{
				// continue the search, check children
				return(remove(pnode->children, gs, idx+1));
			}
		}
		else
		{
			// not found
			break;
		}
	}

	// not found
	return(NOMATCH);
}

template <class DataType>
void
Trie<DataType>::dump(const TrieNode<DataType> *node, 
	GString<DataType> gs, std::ostream &os) const
{
	// check if end of line
	if (node == NULL)
		return;

	// add current letter and continue
	GString<DataType> gssave(gs);
	gs += GString<DataType>(node->data);

	// check if we have a word
	if (node->isaword)
	{
		// print out the word
		os << gs << std::endl;
	}

	// follow children and siblings
	dump(node->children, gs, os);
	dump(node->sibling, gssave, os);
	return;
}

// trie iterator iterator constructors and destructor
template <class DataType>
TrieIterator<DataType>::TrieIterator(const TrieIterator<DataType> &iter): 
	trie(iter.trie), queue(iter.queue)
{
	// do nothing
}

template <class DataType>
TrieIterator<DataType>::TrieIterator(const Trie<DataType> &t): 
	trie(t), queue()
{
	reset();
}

template <class DataType>
TrieIterator<DataType>::~TrieIterator()
{
	// do nothing
}

// reset iterator to beginning
template <class DataType>
void
TrieIterator<DataType>::reset()
{
	queue.clear();
	GString<DataType> gs;
	reset(queue, trie.root, gs);
	return;
}

template <class DataType>
void
TrieIterator<DataType>::reset(Queue_List<GString<DataType> > &q, 
	const TrieNode<DataType> *node, GString<DataType> gs)
{
	// check if end of line
	if (node == NULL)
		return;

	// add current letter and continue
	GString<DataType> gssave(gs);
	gs += GString<DataType>(node->data);

	// check if we have a word
	if (node->isaword)
	{
		// store word in queue
		MustBeTrue(queue.enqueue(gs) == OK);
	}

	// follow children and siblings
	reset(q, node->children, gs);
	reset(q, node->sibling, gssave);
	return;
}


// is list empty
template <class DataType>
int
TrieIterator<DataType>::done() const
{
	return(queue.isEmpty());
}

// return current data
template <class DataType>
GString<DataType>
TrieIterator<DataType>::operator()()
{
	GString<DataType> gs;
	MustBeTrue(queue.front(gs) == OK);
	return(gs);
}

// increment to next data item in list
template <class DataType>
int
TrieIterator<DataType>::operator++(int)
{
	// is iterator done
	if (queue.isEmpty()) return(NOTOK);

	// get next node from queue
	GString<DataType> gs;
	MustBeTrue(queue.dequeue(gs) == OK);

	// all done
	return(OK);
}


