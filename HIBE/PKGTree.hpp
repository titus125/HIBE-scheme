//
//  PKGTree.hpp
//  HIBE
//
//  Created by Titus Wong on 18/8/2020.
//  Copyright © 2020 Titus Wong. All rights reserved.
//

#include<iostream>
using namespace std;
#include <vector>
#include <unordered_map>
#include "HIDParameter.hpp"

class PKG {
    private:
    PKG *root;
    element_t *st; //secret
    element_t *St; //private key
    element_t *Pt;
    vector<element_t*> Qt; //private key, contains {Q0} for root PKG and {Q1,...,Qt} for lower level PKGs
    HIDParameter *param;
    bool extracted;
    
    public:
    ~PKG();
    void rootSetUp();
    void lowerLevelSetUp();
    void extraction(PKG *child);
    tuple<vector<element_t*>,string,string> encrypt(string message);
    string decrypt(tuple<vector<element_t*>,string,string> ciphertext, element_t *St, vector<element_t*> Qt);
    pair<vector<element_t*>,element_t*> sign(string message);
    bool verify(pair<vector<element_t*>,element_t*> signature, string idTuple, string message);
    vector<PKG*> children;
    string idString;
    vector<string> idVector;
    PKG *parent;
    int height;
    
    //helpers
    pair<vector<element_t*>,string> BasicHIDEencrypt(string message,string idString, element_t *key);
    void idAppend(string s);
    bool isAncestor(PKG *child);
    bool inGroup1(element_t *e);
    string getID_1ToID_t(int n,string id);
    
    //These are only for testing purposes
    element_t* getst(); // secret
    element_t* getSt(); // private key
    element_t* getPt();
    vector<element_t*> getQt(); // private key
    void printID();
};

class PKGTree {
    
    private:
    PKG *root;
    int treeHeight;
    unordered_map<string, PKG*> lookUp;
    
    public:
    PKGTree();
    ~PKGTree();
    PKG* lookUpNode(string s);
    void insert(string id, string newId);
    
    //These are only for testing purposes
    void printTree();
    void printTreeHelper(PKG* node);
};
