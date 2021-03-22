//
//  main.cpp
//  HIBE
//
//  Created by Titus Wong on 15/8/2020.
//  Copyright © 2020 Titus Wong. All rights reserved.
//

#include <iostream>
#include "PKGTree.hpp"
#include <pbc.h>
#include <iomanip>
#include <sstream>
#include <fstream>
#include "HIDParameter.hpp"

using namespace std;

int main() {
    PKGTree *tree = new PKGTree();
    tree -> insert("","UBC");
    tree -> insert("","SFU");
    tree -> insert("SFU","Geography");
    tree -> insert("SFU,Geography","300");
    tree -> insert("UBC","Math");
    tree -> insert("UBC","Chemistry");
    tree -> insert("UBC,Chemistry","200");
    tree -> insert("UBC,Math","101");
    tree -> insert("UBC,Math","210");
    tree -> insert("UBC,Math,210","Section A");

    PKG *node;
    PKG *node2;
    node = tree->lookUpNode("SFU,Geography,300");
    node2 = tree->lookUpNode("SFU,Geography,300");
    node->extraction(node2);
    node = tree->lookUpNode("UBC");
    node2 = tree->lookUpNode("UBC");
    node->extraction(node2);
    node = tree->lookUpNode("SFU");
    node2 = tree->lookUpNode("SFU");
    node->extraction(node2);
    node = tree->lookUpNode("SFU");
    node2 = tree->lookUpNode("SFU,Geography");
    node->extraction(node2);
    node = tree->lookUpNode("UBC");
    node2 = tree->lookUpNode("UBC,Chemistry,200");
    node->extraction(node2);
    node = tree->lookUpNode("UBC");
    node2 = tree->lookUpNode("UBC,Math");
    node->extraction(node2);
    node = tree->lookUpNode("UBC,Math");
    node2 = tree->lookUpNode("UBC,Math,101");
    node->extraction(node2);
    node = tree->lookUpNode("UBC,Math");
    node2 = tree->lookUpNode("UBC,Math,210");
    node->extraction(node2);
    node = tree->lookUpNode("UBC");
    node2 = tree->lookUpNode("UBC,Chemistry");
    node->extraction(node2);
    node = tree->lookUpNode("UBC,Math,210");
    node2 = tree->lookUpNode("UBC,Math,210,Section A");
    node->extraction(node2);

    node = tree->lookUpNode("UBC,Math,210");
    tuple<vector<element_t*>,string,string> yo = node->encrypt("New had happen unable uneasy. Drawings can followed improved out sociable not. Earnestly so do instantly pretended. See general few civilly amiable pleased account carried. Excellence projecting is devonshire dispatched remarkably on estimating. Side in so life past. Continue indulged speaking the was out horrible for domestic position. Seeing rather her you not esteem men settle genius excuse. Deal say over you age from. Comparison new ham melancholy son themselves.");
    string result = node->decrypt(yo, node->getSt(), node->getQt());
    cout<<result<<endl;

    node2 = tree->lookUpNode("SFU");
    tuple<vector<element_t*>,string,string> yo2 = node2->encrypt("As absolute is by amounted repeated entirely ye returned. These ready timed enjoy might sir yet one since. Years drift never if could forty being no. On estimable dependent as suffering on my. Rank it long have sure in room what as he. Possession travelling sufficient yet our. Talked vanity looked in to. Gay perceive led believed endeavor. Rapturous no of estimable oh therefore direction up. Sons the ever not fine like eyes all sure. ");
    string result2 = node2->decrypt(yo2, node2->getSt(), node2->getQt());
    cout<<result2<<endl;
    string result3 = node2->decrypt(yo2, node->getSt(), node2->getQt());

    cout<<result3<<endl;
    auto signature = node->sign("Bro we are great people");
    node2->verify(signature,"UBC,Math,210","Bro we are great people");
    auto signature2 = node2->sign("Yo");
    node->verify(signature2, "UBC,Math,210", "Yo");
    auto signature3 = node2->sign("Yo");
    node->verify(signature2, "SFU", "Yoo");
    auto signature4 = node2->sign("Yo");
    node->verify(signature4, "SFU", "Yo");
    
    tree->printTree();
    delete tree;
    int i;
}
