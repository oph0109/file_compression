#include "hashmap.h"
#include "util.h"
#include <iostream>
#include <fstream>
using namespace std;


int main() {
    /*
    hashmap h;
    
    buildFrequencyMap("medium.txt", true, h);

    HuffmanNode * root = buildEncodingTree(h);
    printBT(root);
    map<int, string> code;
    code = buildEncodingMap(root);
    cout << h;
    for(auto e : code) {
        cout << (char)e.first << " " << e.second << endl;
    }
    
    ifstream in("medium.txt");
    ofbitstream out("out.huf");
    int size = 0;
    
    string encoded = encode(in, code, out, size, true);

    ifbitstream ifasd("out.huf");
    ofstream asdfhj("out1.huf");
    cout << endl;
    cout << decode(ifasd, root, asdfhj);
    */
    compress("secretmessage.txt");
    decompress("secretmessage.txt.huf");
    
}