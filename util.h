//
// STARTER CODE: util.h
//
// TODO:  Write your own header and fill in all functions below.
//

#include <iostream>
#include <fstream>
#include <map>
#include <queue>       // std::priority_queue
#include <vector>      // std::vector
#include <functional>  // std::greater
#include <string>
#include "bitstream.h"
#include "hashmap.h"
#include "mymap.h"
#pragma once

struct HuffmanNode {
  int character;
  int count;
  HuffmanNode* zero;
  HuffmanNode* one;
};

class Compare {
 public:
  bool operator()(HuffmanNode* x, HuffmanNode* y) {
    return x->count > y->count;
  }
};

//
// *This method frees the memory allocated for the Huffman tree.
//
void freeTree(HuffmanNode* node) {
  if (node == nullptr) return;

  freeTree(node->zero);
  freeTree(node->one);
  free(node);
}

//
// *This function build the frequency map.  If isFile is true, then it reads
// from filename.  If isFile is false, then it reads from a string filename.
//
void buildFrequencyMap(string filename, bool isFile, hashmap& map) {
  ifstream in;
  char ch;
  if (isFile) {
    in.open(filename);
    in.get(ch);
    while (!in.eof()) {
      if (map.containsKey((int)ch))
        map.put(ch, map.get(ch) + 1);
      else
        map.put(ch, 1);
      in.get(ch);
    }
    map.put(256, 1);
  } else {
    for (int i = 0; i < filename.size(); i++) {
      if (map.containsKey(filename[i]))
        map.put(filename[i], map.get(filename[i]) + 1);
      else
        map.put(filename[i], 1);
    }
    map.put(256, 1);
  }
}

//
// *This function builds an encoding tree from the frequency map.
//
HuffmanNode* buildEncodingTree(hashmap& map) {
  priority_queue<HuffmanNode*, vector<HuffmanNode*>, Compare> pq;
  vector<int> chars = map.keys();
  for (int i = 0; i < chars.size(); i++) {
    HuffmanNode* temp = new HuffmanNode;
    temp->character = chars[i];
    temp->count = map.get(chars[i]);
    temp->zero = nullptr;
    temp->one = nullptr;
    pq.push(temp);
  }

  while (pq.size() != 1) {
    HuffmanNode* l = pq.top();
    pq.pop();
    HuffmanNode* r = pq.top();
    pq.pop();
    HuffmanNode* node = new HuffmanNode;
    node->character = NOT_A_CHAR;
    node->count = l->count + r->count;
    node->zero = l;
    node->one = r;
    pq.push(node);
  }
  return pq.top();
}

// _encodingMapHelper
// recursively travels to every leaf node, and keeps track of the path adding a
// 0 to str if going left, and a 1 if going right
void _encodingMapHelper(HuffmanNode* root, string str,
                        mymap<int, string>& encodingMap) {
  if (root == nullptr) {
    return;
  }

  if (root->zero == nullptr && root->one == nullptr) {
    encodingMap.put(root->character, (str != "") ? str : "1");
  }

  _encodingMapHelper(root->zero, str + "0", encodingMap);
  _encodingMapHelper(root->one, str + "1", encodingMap);
}

//
// *This function builds the encoding map from an encoding tree.
//
// sets up call to the recursive helper function
mymap<int, string> buildEncodingMap(HuffmanNode* tree) {
  mymap<int, string> encodingMap;
  string str = "";
  _encodingMapHelper(tree, str, encodingMap);
  return encodingMap;
}

//
// *This function encodes the data in the input stream into the output stream
// using the encodingMap.  This function calculates the number of bits
// written to the output stream and sets result to the size parameter, which is
// passed by reference.  This function also returns a string representation of
// the output file, which is particularly useful for testing.
//
string encode(ifstream& input, mymap<int, string>& encodingMap,
              ofbitstream& output, int& size, bool makeFile) {
  char ch;
  string str = "";
  int bit;
  if (!input) return str;

  input.get(ch);
  while (!input.eof()) {
    str = str + encodingMap[ch];
    input.get(ch);
  }
  str += encodingMap[PSEUDO_EOF];
  size = str.length();

  for (int i = 0; i < str.length(); i++) {
    bit = str[i] - 48;
    output.writeBit(bit);
  }
  input.close();
  output.close();

  return str;
}

bool isLeaf(HuffmanNode* node) {
  if (node->zero == nullptr && node->one == nullptr) {
    return true;
  } else {
    return false;
  }
}

//
// *This function decodes the input stream and writes the result to the output
// stream using the encodingTree.  This function also returns a string
// representation of the output file, which is particularly useful for testing.
//
string decode(ifbitstream& input, HuffmanNode* encodingTree, ofstream& output) {
  string str;
  char curChar;
  int bit = 0;
  HuffmanNode* cur = encodingTree;

  if (!output) {
    return str;
  }
  while (bit != -1) {
    bit = input.readBit();
    if (bit == 0)
      cur = cur->zero;
    else if (bit == 1)
      cur = cur->one;

    if (isLeaf(cur)) {
      curChar = (char)cur->character;
      if (cur->character == 256) {
        output << str;
        return str;
      }
      str = str + curChar;
      cur = encodingTree;
    }
  }
  output << str << endl;
  input.close();
  output.close();
  return str;
}

//
// *This function completes the entire compression process.  Given a file,
// filename, this function (1) builds a frequency map; (2) builds an encoding
// tree; (3) builds an encoding map; (4) encodes the file (don't forget to
// include the frequency map in the header of the output file).  This function
// should create a compressed file named (filename + ".huf") and should also
// return a string version of the bit pattern.
//
string compress(string filename) {
  hashmap h;
  stringstream ss;
  string ifname = filename;
  string ofname = filename + ".huf";
  buildFrequencyMap(ifname, true, h);
  HuffmanNode* root = buildEncodingTree(h);
  mymap<int, string> encoded;
  encoded = buildEncodingMap(root);
  int size = 0;
  ifstream in(ifname);
  ofbitstream out(ofname);
  out << h;
  string str = encode(in, encoded, out, size, true);
  return str;
}

//
// *This function completes the entire decompression process.  Given the file,
// filename (which should end with ".huf"), (1) extract the header and build
// the frequency map; (2) build an encoding tree from the frequency map; (3)
// using the encoding tree to decode the file.  This function should create a
// compressed file using the following convention.
// If filename = "example.txt.huf", then the uncompressed file should be named
// "example_unc.txt".  The function should return a string version of the
// uncompressed file.  Note: this function should reverse what the compress
// function did.
//
string decompress(string filename) {
  string ifname = filename;
  string ofname = filename.substr(0, filename.length() - 8) + "_unc.txt";
  hashmap h;
  ifstream inFile(filename);
  inFile >> h;
  HuffmanNode* root = buildEncodingTree(h);
  inFile.close();
  ifbitstream in(ifname);
  ofstream out(ofname);

  char dummy = '\0';  // dummy character to detect when header is over
  in.get(dummy);      // skipping until after header
  while (dummy != '}') {
    in.get(dummy);
  }
  return decode(in, root, out);
}
