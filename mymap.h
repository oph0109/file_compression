// mymap.h
//
// TODO: write this file header comment.
#pragma once

#include <iostream>
#include <sstream>
#include <stack>

using namespace std;

template<typename keyType, typename valueType>
class mymap {
 private:
    struct NODE {
        keyType key;  // used to build BST
        valueType value;  // stored data for the map
        NODE* left;  // links to left child
        NODE* right;  // links to right child
        int nL;  // number of nodes in left subtree
        int nR;  // number of nodes in right subtree
        bool isThreaded;
    };
    NODE* root;  // pointer to root node of the BST
    int size;  // # of key/value pairs in the mymap

    //
    // iterator:
    // This iterator is used so that mymap will work with a foreach loop.
    //
    struct iterator {
     private:
        NODE* curr;  // points to current in-order node for begin/end

     public:
        iterator(NODE* node) {
            curr = node;
        }

        keyType operator *() {
            return curr -> key;
        }

        bool operator ==(const iterator& rhs) {
            return curr == rhs.curr;
        }

        bool operator !=(const iterator& rhs) {
            return curr != rhs.curr;
        }

        bool isDefault() {
            return !curr;
        }

        //
        // operator++:
        //
        // This function should advance curr to the next in-order node.
        // O(logN)
        //
        iterator operator++() {
            if(curr->isThreaded) {
		curr = curr->right;
            } else {
		if(curr->right == NULL)
		    return NULL;
		curr = curr->right;
		while(curr->left != NULL) {
		    curr = curr->left;
		}
            }
	    return iterator(curr);
        }
    };

 public:
    //
    // default constructor:
    //
    // Creates an empty mymap.
    // Time complexity: O(1)
    //
    mymap() {
	root = nullptr;
	size = 0;
    }

    //
    // copy constructor:
    //
    // Constructs a new mymap which is a copy of the "other" mymap.
    // Sets all member variables appropriately.
    // Time complexity: O(n), where n is total number of nodes in threaded,
    // self-balancing BST.
    //
    mymap(const mymap& other) {
	root = nullptr;
	size = 0;
	NODE* nRoot = other.root;
	_copy(nRoot);
	size = other.size;
    }

    //
    // operator=:
    //
    // Clears "this" mymap and then makes a copy of the "other" mymap.
    // Sets all member variables appropriately.
    // Time complexity: O(n), where n is total number of nodes in threaded,
    // self-balancing BST.
    //
    mymap& operator=(const mymap& other) {
	if(this == &other)
		return *this;
	clear();
	NODE* nRoot = other.root;
	_copy(nRoot);
	return *this;
    }

    // clear:
    //
    // Frees the memory associated with the mymap; can be used for testing.
    // Time complexity: O(n), where n is total number of nodes in threaded,
    // self-balancing BST.
    //
    void clear() {
	NODE* cur = root;
	_clear(cur);
	root = nullptr;
	size = 0;

    }

    //
    // destructor:
    //
    // Frees the memory associated with the mymap.
    // Time complexity: O(n), where n is total number of nodes in threaded,
    // self-balancing BST.
    //
    ~mymap() {
	_clear(root);
    }

    //
    // put:
    //
    // Inserts the key/value into the threaded, self-balancing BST based on
    // the key.
    // Time complexity: O(logn + mlogm), where n is total number of nodes in the
    // threaded, self-balancing BST and m is the number of nodes in the
    // sub-tree that needs to be re-balanced.
    // Space complexity: O(1)
    //
    void put(keyType key, valueType value) {
	NODE * prev = NULL;
	NODE * cur = root;
	NODE * curv = NULL;
	NODE * prevv = NULL;
	bool kCheck = !contains(key);
	while(cur != NULL) {
	    if(cur->key == key) {
		cur->value = value;
		return;
	    }
	    putSearch(key, cur, prev, kCheck, curv, prevv);
	}
	NODE * temp = new NODE();
	temp->key = key;
	temp->value = value;
	temp->nL = 0;
	temp->nR = 0;
	temp->isThreaded = false;
	size++;
	insert(true, temp, prev, key,  curv, prevv);
    }

    //
    // contains:
    // Returns true if the key is in mymap, return false if not.
    // Time complexity: O(logn), where n is total number of nodes in the
    // threaded, self-balancing BST
    //
    bool contains(keyType key) {
	NODE * cur = root;
	while(cur != NULL) {
	    if(cur->key == key)
		return true;
	    search(key, cur);
	}
	return false;
    }

    //
    // get:
    //
    // Returns the value for the given key; if the key is not found, the
    // default value, valueType(), is returned (but not added to mymap).
    // Time complexity: O(logn), where n is total number of nodes in the
    // threaded, self-balancing BST
    //
    valueType get(keyType key) {
	NODE * cur = root;
	while(cur != NULL) {
	    if(cur->key == key)
		return cur->value;
	    search(key, cur);
        }
	return valueType();
    }

    //
    // operator[]:
    //
    // Returns the value for the given key; if the key is not found,
    // the default value, valueType(), is returned (and the resulting new
    // key/value pair is inserted into the map).
    // Time complexity: O(logn + mlogm), where n is total number of nodes in the
    // threaded, self-balancing BST and m is the number of nodes in the
    // sub-trees that need to be re-balanced.
    // Space complexity: O(1)
    //
    valueType operator[](keyType key) {
	return get(key);
    }

    //
    // Size:
    //
    // Returns the # of key/value pairs in the mymap, 0 if empty.
    // O(1)
    //
    int Size() {
        return size;
    }

    //
    // begin:
    //
    // returns an iterator to the first in order NODE.
    // Time complexity: O(logn), where n is total number of nodes in the
    // threaded, self-balancing BST
    //
    iterator begin() {
	NODE * cur = root;

	while(cur != NULL) {
	    cur = cur->left;
	}
	return iterator(cur);
    }

    //
    // end:
    //
    // returns an iterator to the last in order NODE.
    // this function is given to you.
    // 
    // Time Complexity: O(1)
    //
    iterator end() {
        return iterator(nullptr);
    }

    //
    // toString:
    //
    // Returns a string of the entire mymap, in order.
    // Format for 8/80, 15/150, 20/200:
    // "key: 8 value: 80\nkey: 15 value: 150\nkey: 20 value: 200\n
    // Time complexity: O(n), where n is total number of nodes in the
    // threaded, self-balancing BST
    //
    string toString() {
	stringstream ss("");
	_string(root, ss);
	return ss.str();
    }

    //
    // toVector:
    //
    // Returns a vector of the entire map, in order.  For 8/80, 15/150, 20/200:
    // {{8, 80}, {15, 150}, {20, 200}}
    // Time complexity: O(n), where n is total number of nodes in the
    // threaded, self-balancing BST
    //
    vector<pair<keyType, valueType> > toVector() {
	vector<pair<keyType, valueType> > vec;
	_buildVec(root, vec);
	return vec;
	
    }

    //
    // checkBalance:
    //
    // Returns a string of mymap that verifies that the tree is properly
    // balanced.  For example, if keys: 1, 2, 3 are inserted in that order,
    // function should return a string in this format (in pre-order):
    // "key: 2, nL: 1, nR: 1\nkey: 1, nL: 0, nR: 0\nkey: 3, nL: 0, nR: 0\n";
    // Time complexity: O(n), where n is total number of nodes in the
    // threaded, self-balancing BST
    //
    string checkBalance() {
	stringstream ss("");
	_string(root, ss);
	return ss.str();
    }

    //
    //	checkNode:
    //  	Checks if the node being inserted will cause the tree to become un balanced
    //	returns false if it will disrupt the balance
    //
    bool checkNode(NODE* cur) {
        int minH = min(cur->nL, cur->nR);
        int maxH = max(cur->nL, cur->nR);
        if (maxH <= (minH * 2) + 1)
            return true;
      return false;
    }

    //
    // putSearch:
    // saves cur in prev, and then checks for balance
    // updates violator node along with its parent if it breaks balancing
    //
    void putSearch(keyType &key, NODE* &cur, NODE* &prev, bool kCheck, NODE* &curV, NODE* &prevV) {
        if(key < cur->key) {
            if(kCheck) {
                cur->nL++;
            }
            if(!checkNode(cur) && !curV) {
                prevV = prev;
                curV = cur;
            }
            prev = cur;
            cur = cur->left;
        } else {
            if(kCheck)
		cur->nR++;
	    if(!checkNode(cur) && !curV) {
		prevV = prev;
		curV = cur;
	    }
            prev = cur;
            if(cur->isThreaded)
		cur = NULL;
	    else
		cur = cur->right;
        }
    }
    //
    // _buildVec:
    // recursive helper function which uses in-order traversal
    //
    void _buildVec(NODE * cur, vector<pair<keyType, valueType>> &vec) {
	if(cur == NULL)
	    return;
	_buildVec(cur->left, vec);
    pair<keyType, valueType> temp(cur->key, cur->value);
    vec.push_back(temp);
	if(cur->isThreaded)
	    _buildVec(NULL, vec);
	else
	    _buildVec(cur->right, vec);
    }

    void _nodeVec(NODE * cur, vector<NODE*> &vec) {
        if(cur == NULL)
            return;
        _nodeVec(cur->left, vec);
        vec.push_back(cur);
        if(cur->isThreaded)
            _nodeVec(NULL, vec);
        else
            _nodeVec(cur->right, vec);
    }
    //
    // nodeVec:
    // uses recursive helper function in order to return a vector of nodes from a BST
    //
    vector<NODE*> nodeVec(NODE* cur) {
	vector<NODE*> vec;
	_nodeVec(cur,vec);
	return vec;
    }

    //
    // insert:
    // inserts nodes into the BST
    //
    void insert(bool bal, NODE * &n, NODE* &prev, keyType key, NODE* &curv, NODE* &prevv) {
	if(prev == NULL) {
            root = n;
            root->right = NULL;
        } else if (prev->key > key) {
 	    prev->left = n;
	    n->right = prev;
	    n->isThreaded = true;           
	} else {
	    n->right = prev->right;
	    prev->right = n;
	    prev->isThreaded = false;
	    n->isThreaded = true;
	}
	if(bal && curv != NULL) {
	    vector<NODE*> vec = nodeVec(curv);
	    NODE* nTree = _balanceT(0, vec.size() - 1, vec, NULL);
            if(prevv == NULL)
                root = nTree;
            else if(nTree->key < prevv->key)
                prevv->left = nTree;
            else 
                prevv->right = nTree;
	}
    }
    //
    //	_balanceT:
    //	recursive helper function for balancing the tree
    //	uses left right and middle and vectors in order to find the new root node
    //	
    NODE* _balanceT(int l, int r, vector<NODE*> vec, NODE* rT) {
        if(l > r)
            return NULL;
        int m = (l + r) / 2;
        NODE * nRoot = vec[m];
        nRoot->nL = 0;
        nRoot->nR = 0;
        nRoot->isThreaded = false;
        nRoot->left = _balanceT(l, m - 1, vec, nRoot);
        nRoot->right = _balanceT(m + 1, r, vec, rT);
        if(nRoot->left != NULL)
	    nRoot->nL = (nRoot->left->nL + nRoot->left->nR + 1);
        if(nRoot->right != NULL) {
	    nRoot->nR = (nRoot->right->nL + nRoot->right->nR + 1);
	}

	if(nRoot->right == NULL) {
	    nRoot->right = rT;
	    nRoot->isThreaded = true;
	}
    return nRoot;
    }

    //
    // search;
    // helper function which determines how to proceed (left or right)
    //
    void search(keyType key, NODE* &cur) {
	if(cur->key <= key) {
	    if(cur->isThreaded)
		cur = NULL;
	    else
		cur = cur->right;
	} else
	    cur = cur->left;
    }

    //
    //	_clear:
    //	recursive helper function which clears all of the nodes in the BST
    //
    void _clear(NODE* cur) {
	if(cur == NULL)
	    return;
	_clear(cur->left);
	if(!cur->isThreaded)
		_clear(cur->right);
	delete cur;
    } 

    //
    // _copy
    // recursive copy function to make a deep copy of a BST
    //
    void _copy(NODE* other) {
    if(other == nullptr)
	return;
    NODE * prev = NULL;
    NODE * cur = root;
    NODE * curv = NULL;
    NODE * prevv = NULL;
    bool kCheck = !contains(other->key);
    while(cur != NULL) {
        if(cur->key == other->key) {
            cur->value = other->value;
            return;
        }
            putSearch(other->key, cur, prev, kCheck, curv, prevv);
    }
    NODE * temp = new NODE();
    temp->key = other->key;
    temp->value = other->value;
    temp->nL = 0;
    temp->nR = 0;
    temp->isThreaded = false;
    size++;
    insert(false, temp, prev, other->key, curv, prevv);
    _copy(other->left);
    if(!other->isThreaded)
	_copy(other->right);
    }		

    //
    //	_string:
    //	recursive helper function which converts BST to stringstream
    //
    void _string(NODE* cur, ostream& out) {
	if(cur == NULL)
	    return;
        _string(cur->left, out);
	out << "key: " << cur->key;
	out << " value: " << cur->value << "\n";
	if(cur->isThreaded)
	    cur = NULL;
	else
	    _string(cur->right, out);
    }

    //
    // _checkBalance
    //	recursive helper function to build string using pre-order traversal
    //
    void _checkBalance(NODE* cur, ostream &out) {
	if(cur == NULL)
	    return;
	out << "key: " << cur->key << ", nL: " << cur->nL << ", nR: " << cur->nR << "\n";
	_checkBalance(cur->left, out);
	if(cur->isThreaded)
	    _checkBalance(NULL, out);
	else
	    _checkBalance(cur->right, out);
    }
 
};

    

