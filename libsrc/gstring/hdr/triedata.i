//
// Copyright (C) 2010, OMBT LLC and Mike A. Rumore
// All rights reserved.
// Contact: Mike A. Rumore, (mike.a.rumore@gmail.com)
//
// member functions for a trie

// trie node constructors and destructor
template <class CharacterType, class PayloadType>
TrieDataNode<CharacterType, PayloadType>::TrieDataNode(const CharacterType &d):
	isaword(false), data(d), sibling(NULL), children(NULL)
{
	// do nothing
}

template <class CharacterType, class PayloadType>
TrieDataNode<CharacterType, PayloadType>::TrieDataNode(const TrieDataNode<CharacterType, PayloadType> &tn):
	isaword(tn.isaword), data(tn.data), sibling(NULL), children(NULL)
{
	// do nothing
}

template <class CharacterType, class PayloadType>
TrieDataNode<CharacterType, PayloadType>::~TrieDataNode()
{
	isaword = false;
	sibling = NULL;
	children = NULL;
}

// assignment
template <class CharacterType, class PayloadType>
TrieDataNode<CharacterType, PayloadType> &
TrieDataNode<CharacterType, PayloadType>::operator=(const TrieDataNode<CharacterType, PayloadType> &tn)
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
template <class CharacterType, class PayloadType>
std::ostream &
operator<<(std::ostream &os, TrieDataNode<CharacterType, PayloadType> &tn)
{
	os << tn.data;
	return(os);
}

// trie constructors and destructor
template <class CharacterType, class PayloadType>
TrieData<CharacterType, PayloadType>::TrieData():
	root(NULL)
{
	// do nothing
}

template <class CharacterType, class PayloadType>
TrieData<CharacterType, PayloadType>::TrieData(const TrieData<CharacterType, PayloadType> &t):
	root(NULL)
{
	root = t.copy();
}

template <class CharacterType, class PayloadType>
TrieData<CharacterType, PayloadType>::~TrieData()
{
	clear();
}

// assignment
template <class CharacterType, class PayloadType>
TrieData<CharacterType, PayloadType> &
TrieData<CharacterType, PayloadType>::operator=(const TrieData<CharacterType, PayloadType> &t)
{
	if (this != &t)
	{
		clear();
		root = t.copy();
	}
	return(*this);
}

// trie (prefix tree) operations
template <class CharacterType, class PayloadType>
int
TrieData<CharacterType, PayloadType>::insert(const GString<CharacterType> &gs, const PayloadType &payload)
{
	// start scanning sibling list, look for first letter.
	TrieDataNode<CharacterType, PayloadType> *proot = root;
	TrieDataNode<CharacterType, PayloadType> *pprev = NULL;
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
				proot->payload = payload;
				return(OK);
			}
			else
			{
				// continue the search, check children
				return(insert(proot->children, gs, idx+1, payload));
			}
		}
		else
		{
			break;
		}
	}

	// not found, create a new path to the new word
	TrieDataNode<CharacterType, PayloadType> *pnew = NULL;
	if (createpath(pnew, gs, idx, payload) != OK)
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

template <class CharacterType, class PayloadType>
int
TrieData<CharacterType, PayloadType>::insert(TrieDataNode<CharacterType, PayloadType> *&pchild, const GString<CharacterType> &gs, int idx, const PayloadType &payload)
{
	// look for a match
	if (pchild == NULL)
	{
		// word was not found, add new path
		return(createpath(pchild, gs, idx, payload));
	}

	// start scanning sibling list
	TrieDataNode<CharacterType, PayloadType> *pch = pchild;
	TrieDataNode<CharacterType, PayloadType> *pprev = NULL;
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
				pch->payload = payload;
				return(OK);
			}
			else
			{
				// continue the search, check children
				return(insert(pch->children, gs, idx+1, payload));
			}
		}
		else
		{
			break;
		}
	}

	// not found, create a new path to the new word
	TrieDataNode<CharacterType, PayloadType> *pnew = NULL;
	if (createpath(pnew, gs, idx, payload) != OK)
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

template <class CharacterType, class PayloadType>
int
TrieData<CharacterType, PayloadType>::createpath(TrieDataNode<CharacterType, PayloadType> *&pnode, const GString<CharacterType> &gs, int idx, const PayloadType &payload)
{
	// must not be at the end
	assert(idx < gs.gstrlen());

	// allocate a new node
	pnode = new TrieDataNode<CharacterType, PayloadType>(gs[idx]);
	assert(pnode != NULL);

	// continue ?
	if ((idx+1) >= gs.gstrlen())
	{
		// at the end of a word
		pnode->isaword = true;
		pnode->payload = payload;
		return(OK);
	}
	else
	{
		// do the next node
		return(createpath(pnode->children, gs, idx+1, payload));
	}
}

template <class CharacterType, class PayloadType>
int
TrieData<CharacterType, PayloadType>::remove(const GString<CharacterType> &gs, bool recursive)
{
	// start scanning sibling list
	TrieDataNode<CharacterType, PayloadType> *proot = root;
	TrieDataNode<CharacterType, PayloadType> *pprev = NULL;
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
				if (recursive)
				{
					// remove node and all below it.
					if (pprev == NULL)
					{
						// node is the first node in sibling list
						pprev = root->sibling;
						clear(root);
						root = pprev;
					}
					else
					{
						// remove internal or end node
						pprev->sibling = proot->sibling;
						clear(proot);
					}
				}
				else if (proot->children != NULL)
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
				return(remove(proot->children, gs, idx+1, recursive));
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

template <class CharacterType, class PayloadType>
int
TrieData<CharacterType, PayloadType>::remove(TrieDataNode<CharacterType, PayloadType> *&pchild, const GString<CharacterType> &gs, int idx, bool recursive)
{
	// look for a match
	if (pchild == NULL || (idx >= gs.gstrlen()))
	{
		// word was not found
		return(NOMATCH);
	}

	// start scanning sibling list
	TrieDataNode<CharacterType, PayloadType> *pnode = pchild;
	TrieDataNode<CharacterType, PayloadType> *pprev = NULL;
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
				if (recursive)
				{
					if (pprev == NULL)
					{
						// node is // the first node in sibling list
						pprev = pchild->sibling;
						clear(pchild);
						pchild = pprev;
					}
					else
					{
						// remove internal or end node
						pprev->sibling = pnode->sibling;
						clear(pnode);
					}
				}
				else if (pnode->children != NULL)
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
				return(remove(pnode->children, gs, idx+1, recursive));
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

template <class CharacterType, class PayloadType>
bool
TrieData<CharacterType, PayloadType>::includes(const GString<CharacterType> &gs) const
{
	// start scanning sibling list
	TrieDataNode<CharacterType, PayloadType> *proot = root;
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

template <class CharacterType, class PayloadType>
bool
TrieData<CharacterType, PayloadType>::includes( const TrieDataNode<CharacterType, PayloadType> *pnode, const GString<CharacterType> &gs, int idx) const
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

template <class CharacterType, class PayloadType>
int
TrieData<CharacterType, PayloadType>::read(const GString<CharacterType> &gs, PayloadType &payload) const
{
	// start scanning sibling list
	TrieDataNode<CharacterType, PayloadType> *proot = root;
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
				payload = proot->payload;
				return(OK);
			}
			else
			{
				// continue the search, check children
				return(read(proot->children, gs, idx+1, payload));
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

template <class CharacterType, class PayloadType>
int
TrieData<CharacterType, PayloadType>::read( const TrieDataNode<CharacterType, PayloadType> *pnode, const GString<CharacterType> &gs, int idx, PayloadType &payload) const
{
	// look for a match
	if (pnode == NULL || (idx >= gs.gstrlen()))
	{
		// word was not found
		return(NOMATCH);
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
				payload = pnode->payload;
				return(OK);
			}
			else
			{
				// continue the search, check children
				return(read(pnode->children, gs, idx+1, payload));
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

template <class CharacterType, class PayloadType>
int
TrieData<CharacterType, PayloadType>::update(const GString<CharacterType> &gs, const PayloadType &payload) 
{
	// start scanning sibling list
	TrieDataNode<CharacterType, PayloadType> *proot = root;
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
				proot->payload = payload;
				return(OK);
			}
			else
			{
				// continue the search, check children
				return(update(proot->children, gs, idx+1, payload));
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

template <class CharacterType, class PayloadType>
int
TrieData<CharacterType, PayloadType>::update(TrieDataNode<CharacterType, PayloadType> *pnode, const GString<CharacterType> &gs, int idx, const PayloadType &payload)
{
	// look for a match
	if (pnode == NULL || (idx >= gs.gstrlen()))
	{
		// word was not found
		return(NOMATCH);
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
				pnode->payload = payload;
				return(OK);
			}
			else
			{
				// continue the search, check children
				return(update(pnode->children, gs, idx+1, payload));
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

template <class CharacterType, class PayloadType>
int
TrieData<CharacterType, PayloadType>::isEmpty() const
{
	return(root == NULL);
}

template <class CharacterType, class PayloadType>
void
TrieData<CharacterType, PayloadType>::clear()
{
	// clear trie
	clear(root);
	return;
}

template <class CharacterType, class PayloadType>
int
TrieData<CharacterType, PayloadType>::getChildren(const GString<CharacterType> &gs, std::list<CharacterType> &children) const
{
	// assume no children
	children.clear();

	// start scanning sibling list
	TrieDataNode<CharacterType, PayloadType> *proot = root;
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
				// word was found. get any children
				if (proot->children != NULL)
				{
					TrieDataNode<CharacterType, PayloadType> *pchild = proot->children;
					for ( ; pchild != NULL; pchild = pchild->sibling)
					{
						children.push_back(pchild->data);
					}
				}
				return(OK);
			}
			else
			{
				// continue the search, check children
				return(getChildren(proot->children, gs, idx+1, children));
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

template <class CharacterType, class PayloadType>
int
TrieData<CharacterType, PayloadType>::getChildren( const TrieDataNode<CharacterType, PayloadType> *pnode, const GString<CharacterType> &gs, int idx, std::list<CharacterType> &children) const
{
	// look for a match
	if (pnode == NULL || (idx >= gs.gstrlen()))
	{
		// word was not found
		return(NOMATCH);
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
				// word was found. get any children
				if (pnode->children != NULL)
				{
					TrieDataNode<CharacterType, PayloadType> *pchild = pnode->children;
					for ( ; pchild != NULL; pchild = pchild->sibling)
					{
						children.push_back(pchild->data);
					}
				}
				return(OK);
			}
			else
			{
				// continue the search, check children
				return(getChildren(pnode->children, gs, idx+1, children));
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

// print binary tree data
template <class CharacterType, class PayloadType>
std::ostream &
TrieData<CharacterType, PayloadType>::dump(std::ostream &os) const
{
	GString<CharacterType> gs;
	dump(root, gs, os);
	return(os);
}

// output function
template <class CharacterType, class PayloadType>
std::ostream &
operator<<(std::ostream &os, TrieData<CharacterType, PayloadType> &t)
{
	t.dump(os);
	return(os);
}


// utility functions
template <class CharacterType, class PayloadType>
TrieDataNode<CharacterType, PayloadType> *
TrieData<CharacterType, PayloadType>::copy() const
{
	return(copy(root));
}

template <class CharacterType, class PayloadType>
TrieDataNode<CharacterType, PayloadType> *
TrieData<CharacterType, PayloadType>::copy(const TrieDataNode<CharacterType, PayloadType> *pnode) const
{
	TrieDataNode<CharacterType, PayloadType> *pnew = NULL;
	if (pnode != NULL)
	{
		pnew = new TrieDataNode<CharacterType, PayloadType>(*pnode);
		assert(pnew != NULL);
		pnew->sibling = copy(pnode->sibling);
		pnew->children = copy(pnode->children);
	}
	return(pnew);
}

template <class CharacterType, class PayloadType>
void
TrieData<CharacterType, PayloadType>::clear(TrieDataNode<CharacterType, PayloadType> *&pnode)
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


template <class CharacterType, class PayloadType>
void
TrieData<CharacterType, PayloadType>::dump(const TrieDataNode<CharacterType, PayloadType> *node, 
	GString<CharacterType> gs, std::ostream &os) const
{
	// check if end of line
	if (node == NULL)
		return;

	// add current letter and continue
	GString<CharacterType> gssave(gs);
	gs += GString<CharacterType>(node->data);

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

