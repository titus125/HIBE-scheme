//
//  PKGTree.cpp
//  HIBE
//
//  Created by Titus Wong on 18/8/2020.
//  Copyright © 2020 Titus Wong. All rights reserved.
//

#include<iostream>
using namespace std;
#include "PKGTree.hpp"

PKG::~PKG() {
    element_clear(*(this->Pt));
    element_clear(*(this->st));
    element_clear(*(this->St));
    for (element_t *e:this->Qt) {
        element_clear(*e);
        delete e;
    }
    delete this->Pt,this->st,this->St,parent;
    for (PKG *node:children) {
        delete node;
        node = NULL;
    }
    if (height == 0) {
        delete param;
    }
}

void PKG::printID() {
//    std::cout<<idString<<" has a secret of "<<std::endl;
//    element_printf("%B\n", *(this->st));
//    std::cout<<"St:"<<std::endl;
//    element_printf("%B\n", *(this->St));
//    std::cout<<"Pt:"<<std::endl;
//    element_printf("%B\n", *(this->Pt));
//    std::cout<<"Qt:"<<std::endl;
//    if (!this->Qt.empty()) {
//        for (int i = 0; i< this->height-1; i++) {
//            cout<<"Qt["<<i<<"]: "<<endl;
//            element_printf("%B\n", *(this->Qt[i]));
//        }
//    }
}

bool PKG::isAncestor(PKG *child) {
    if (this->height > child->height) {
        return false;
    } else {
        return (child = this) || (child->parent == this) || isAncestor(child->parent);
    }
}

void PKG::idAppend(string s) {
    if (idString == "") {
        idString = s;
    } else {
        idString.append(",");
        idString.append(s);
    }
}

void PKG::rootSetUp() {
    root = this;
    this -> idString = "";
    this -> height = 0;
    
    this->param = new HIDParameter();
    
    this->Pt = new element_t[1];
    element_init_G1(*(this->Pt), *(param->pairing));
    element_set(*(this->Pt), *(param->getP0()));
    
    this->st = new element_t[1];
    element_init_Zr(*(this->st), *(param->pairing));
    element_set(*(this->st), *(param->gets0()));
    
    element_t *e = new element_t[1];
    element_init_G1(*e, *(param->pairing));
    this->Qt.push_back(e);
    element_set(*(this->Qt[0]), *(param->getQ0()));
    
    this->St = new element_t[1];
    element_init_G1(*(this->St), *(param->pairing));
    element_set0(*(this->St));
    
    this->extracted = true;
}

void PKG::lowerLevelSetUp() {
    this->param = this->parent->param;
    this->st = new element_t[1];
    element_init_Zr(*(this->st),*(param->pairing));
    element_random(*(this->st));
    
    //set Pt = H1(ID1,...,IDt)
    this->Pt = new element_t[1];
    element_init_G1(*(this->Pt), *(param->pairing));
    element_set(*(this->Pt), *(param->H1(this->idString)));
    
    //fill Qt with elements
    for (int i = 0; i < height - 1; i++) {
        element_t *e = new element_t[1];
        element_init_G1(*e, *(param->pairing));
        Qt.push_back(e);
    }
}

void PKG::extraction(PKG *child) {
    
    //check if node is an ancestor of child
    if (!this->isAncestor(child)) {
        cout<<"HES ADOPTED"<<endl;
        return;
    }
    
    //check if node is already extracted
    if (child->extracted) {
        cout<<"child is already extracted"<<endl;
        return;
    }
    
    if (this == child) {
        //case: ancestor = child, St = s0*P1+,...,+s_t-1*Pt. As well as Q_t-1 = s_t-1*P0,...,Q_1 = s1*P0
        element_t temp3;
        element_init_G1(temp3,*(param->pairing));
        element_set0(temp3);
    
        for (PKG *cur = child; cur->height != 0; cur = cur->parent) {
            element_t temp2;
            element_init_G1(temp2,*(param->pairing));
            element_mul_zn(temp2, *(cur->Pt), *(cur->parent->st));
            element_add(temp3, temp3, temp2);
            element_clear(temp2);
        }
        
        // set St to be temp
        child->St = new element_t[1];
        element_init_G1(*(child->St), *(param->pairing));
        element_set(*(child->St),temp3);
        element_clear(temp3);
    
        for (PKG *cur = child->parent; cur->height != 0;
            cur = cur->parent) {
            element_t temp;
            element_init_G1(temp, *(param->pairing));
            element_mul_zn(temp, *(param->getP0()), *(cur->st));
            //child->Qt[cur->height-1] = temp;
            element_set(*(child->Qt[cur->height-1]), temp);
            element_clear(temp);
        }
    } else {
        //check if parent is extracted
        if (!this->extracted) {
            cout<<"parent not yet extracted"<<endl;
            return;
        }
    
        //set S_t = S_i + sum of s_k-1 * P_k from i+1 to t
        child->St = new element_t[1];
        element_t temp;
        element_init_G1(temp,*(param->pairing));
        element_set(temp,*(this->St));
    
        // temp += s_k-1 * P_k from k = i+1 to t
        for (PKG *cur = child; cur != this; cur = cur->parent) {
            element_t temp2;
            element_init_G1(temp2,*(param->pairing));
            element_mul_zn(temp2, *(cur->Pt), *(cur->parent->st));
            element_add(temp, temp, temp2);
            element_clear(temp2);
        }
    
        // set St to be temp
        child->St = new element_t[1];
        element_init_G1(*(child->St), *(param->pairing));
        element_set(*(child->St),temp);
        element_clear(temp);
    
        // parent puts Q_1,...,Q_i-1 into Qt[0],...Qt[i-2]
        for (int i = 0; i < this->Qt.size(); i++) {
            element_set(*(child->Qt[i]), *(this->Qt[i]));
        }
    
        //put Q_i+1,...Q_t-1 into Q[i],...Q[t-2]
        for (PKG *cur = child->parent; cur != this; cur = cur->parent) {
            element_t temp;
            element_init_G1(temp, *(param->pairing));
            element_mul_zn(temp, *(param->getP0()), *(cur->st));
            element_set(*(child->Qt[cur->height-1]),temp);
            element_clear(temp);
        }
        //put Q_i into Q[i-1] if parent is not root
        if (this->height > 0) {
            element_t temp2;
            element_init_G1(temp2, *(param->pairing));
            element_mul_zn(temp2, *(param->getP0()), *(this->st));
            element_set(*(child->Qt[this->height-1]),temp2);
            element_clear(temp2);
        }
    }
    child->extracted = true;
    
}

tuple<vector<element_t*>,string,string> PKG::encrypt(string message) {
    
    //setup sigma = H2(random GT element)
    element_t random;
    element_init_GT(random, *(param->pairing));
    element_random(random);
    string sigma = param->H2(random);
    element_clear(random);
    
    //resize message
    message.resize(param->messageSpace*2);
    
    //setup r
    element_t *r = param->H3(sigma,message);
    
    // let p = P1
    element_t p;
    element_init_G1(p, *(param->pairing));
    PKG *start = this;
    for (int i = 1; i < this->height; i++) {
        start = start->parent;
    }
    element_set(p,*(start->Pt));
    
    
    // setup g = e(Q0,P1)
    element_t *g = new element_t[1];
    element_init_GT(*g,*(param->pairing));
    element_pairing(*g,*(param->getQ0()),p);
    
    // ret1[0] points to r * P0
    vector<element_t*> ret1;
    for (int i = 0; i < this->height; i++) {
        element_t *temp2 = new element_t[1];
        element_init_G1(*temp2, *(param->pairing));
        ret1.push_back(temp2);
    }
    element_mul_zn(*ret1[0], *(param->getP0()), *r);
    
    // ret1[1],...,ret[height] points to r * P2,..., r * Pt
    PKG *start2 = this;
    for (int i = this->height-1; i > 0; i--) {
        element_t temp;
        element_init_G1(temp,*(param->pairing));
        element_mul_zn(temp, *(start2->Pt), *r);
        element_set(*ret1[i],temp);
        element_clear(temp);
        start2 = start2 -> parent;
    }
    
    // temp = H2(g^r)
    element_t temp;
    element_init_GT(temp,*(param->pairing));
    element_mul_zn(temp,*g,*r);
    string hash = param->H2(temp);
    element_clear(*r);
    element_clear(*g);
    
    // ret2 = sigma XOR H2(g^r)
    string ret2 = param->XOR(sigma,hash);
    
    //ret3 = M XOR H4(sigma)
    string ret3 = param->XOR(message,param->H4(sigma));
    
    tuple <vector<element_t*>,string,string> ret ;
    get<0>(ret) = ret1;
    get<1>(ret) = ret2;
    get<2>(ret) = ret3;

    element_clear(temp);

    return ret;
}

string PKG:: decrypt(tuple<vector<element_t*>,string,string> ciphertext, element_t *St, vector<element_t*> Qt) {
    
    //check if U0,U2...,Ut are all in G1
    for (element_t* e:get<0>(ciphertext)) {
        if (!inGroup1(e)) {
            cout<<"Ciphertext rejected"<<endl;
            return "";
        }
    }
    cout<<"Ciphertext accepted"<<endl;
    
    //compute e(U0,St)
    element_t temp;
    element_init_GT(temp,*(param->pairing));
    element_pairing(temp,*get<0>(ciphertext)[0],*St);
    
    //compute e(Q_1,U_2) * ,..., * e(Q_t-1,U_t)
    element_t temp2;
    element_t acc;
    element_init_GT(temp2,*(param->pairing));
    element_init_GT(acc,*(param->pairing));
    element_set1(acc);
    
    for (int i = 0; i < Qt.size(); i++) {
        element_pairing(temp2,*Qt[i],*get<0>(ciphertext)[i+1]);
        element_mul(acc,temp2,acc);
    }
    
    element_clear(temp2);
    
    //temp = temp / acc
    element_div(temp, temp, acc);

    string sigma = param->XOR(get<1>(ciphertext),param->H2(temp));
    
    //compute M = W XOR H4(sigma)
    string ret = param->XOR(get<2>(ciphertext),param->H4(sigma));
    
    element_clear(temp);
    element_clear(acc);
    
    //check BasicHIDEencrypt(sigma,H3(sigma,M)) = [U0,...Ut,V]
    pair<vector<element_t*>,string> cipher = BasicHIDEencrypt(sigma,idString,param->H3(sigma,ret));
    bool valid = true;
    for (int i = 0; i < Qt.size(); i++) {
        if (element_cmp(*cipher.first[i], *get<0>(ciphertext)[i]))
            valid = false;
    }
    if (cipher.second != get<1>(ciphertext)) {
        valid = false;
    }
    if (valid) {
        return ret;
    } else {
        cout<< "Ciphertext rejected"<<endl;
        return "";
    }
}

bool PKG::inGroup1(element_t *e) {
    //check Q0 is in G1,i.e y^2 = x^3 + x
    element_t x;
    element_t y;
    element_t lhs;
    element_t rhs;
    element_init_same_as(x, element_x(*e));
    element_init_same_as(y, element_y(*e));
    element_init_same_as(lhs, element_y(*e));
    element_init_same_as(rhs, element_x(*e));
    
    element_set(x,element_x(*e));
    element_set(y,element_y(*e));
    element_square(lhs, y);
    element_square(rhs, x);
    element_mul(rhs, rhs, x);
    element_add(rhs, x, rhs);
    bool valid = !element_cmp(rhs, lhs);
    
    element_clear(x);
    element_clear(y);
    element_clear(lhs);
    element_clear(rhs);
    
    return valid;
}

pair<vector<element_t*>,string> PKG::BasicHIDEencrypt(string message,string idTuple,element_t *key) {
    
    // let p = P1
    element_t p;
    element_init_G1(p, *(param->pairing));
    element_set(p,*(param->H1(getID_1ToID_t(1,idTuple))));

    // setup g = e(Q0,P1)
    element_t *g = new element_t[1];
    element_init_GT(*g,*(param->pairing));
    element_pairing(*g,*(param->getQ0()),p);

    // ret1[0] points to r * P0
    vector<element_t*> ret1;
    for (int i = 0; i < this->height; i++) {
        element_t *stub = new element_t[1];
        element_init_G1(*stub, *(param->pairing));
        ret1.push_back(stub);
    }
    element_mul_zn(*ret1[0], *(param->getP0()), *key);

    // ret1[1],...,ret[height] points to r * P2,..., r * Pt
    PKG *start2 = this;
    for (int i = this->height-1; i > 0; i--) {
        element_t temp;
        element_init_G1(temp,*(param->pairing));
        element_mul_zn(temp, *(start2->Pt), *key);
        element_set(*ret1[i],temp);
        element_clear(temp);
        start2 = start2 -> parent;
    }

    // hash = H2(g^r)
    element_t temp;
    element_init_GT(temp,*(param->pairing));
    element_mul_zn(temp,*g,*key);
    string hash = param->H2(temp);
    element_clear(*key);
    element_clear(*g);
    
    // V = M XOR H2(g^r)
    string ret2 = param->XOR(message, hash);
    
    pair <vector<element_t*>,string> ret ;
    ret.first = ret1;
    ret.second = ret2;

    element_clear(temp);
    return ret;
}

pair<vector<element_t*>,element_t*> PKG::sign(string message) {
    
    //Pm = H1(ID1,...,IDt,M)
    string s = idString;
    s.append(",");
    s.append(message);
    element_t Pm;
    element_init_G1(Pm,*(param->pairing));
    element_set(Pm,*param->H5(s));
    
    //Sig(ID-tuple, M) = St + st * Pm
    element_t* sig = new element_t[1];
    element_init_G1(*sig, *(param->pairing));
    element_mul_zn(*sig, Pm, *(this->st));
    element_add(*sig, *sig, *(this->St));
    element_clear(Pm);
    
    //Add Qt to {Q_1,...,Q_t-1}
    vector<element_t*> ret1;
    ret1 = this->Qt;
    element_t *newQt = new element_t[1];
    element_init_G1(*newQt, *(param->pairing));
    element_mul_zn(*newQt, *(param->getP0()),*(this->st));
    ret1.push_back(newQt);
    
    //return {[Q1,...,Qt],sig}
    pair<vector<element_t*>,element_t*> ret;
    ret.first = ret1;
    ret.second = sig;
    
    return ret;
}

bool PKG::verify(pair<vector<element_t*>,element_t*> signature, string idTuple, string message) {
    
    //compute e(P0,sig)
    element_t lhs;
    element_init_GT(lhs, *(param->pairing));
    element_pairing(lhs, *(param->getP0()), *signature.second);
    
    //compute e(Q0,P1)
    element_t rhs;
    element_init_GT(rhs,*(param->pairing));
    element_pairing(rhs,*(param->getQ0()),*(param->H1(getID_1ToID_t(1,idTuple))));
    
    
    //Pm = H1(ID1,...,IDt,M)
    string s = idTuple;
    s.append(",");
    s.append(message);
    element_t Pm;
    element_init_G1(Pm,*(param->pairing));
    element_set(Pm,*param->H5(s));
    
    //compute e(Qt,Pm)
    element_t temp;
    element_init_GT(temp,*(param->pairing));
    element_pairing(temp,*(signature.first[signature.first.size()-1]),Pm);
    element_mul(rhs, rhs, temp);
    element_clear(temp);
    element_clear(Pm);
    
    //compute e(Q1,P2)*...*e(Q_t-1,P_t)
     element_t temp2;
     element_init_GT(temp2,*(param->pairing));
     
    
    for (int i = 0; i < signature.first.size()-1; i++) {
        element_pairing(temp2,*(signature.first[i]),*(param->H1(getID_1ToID_t(i+2,idTuple))));
        element_mul(rhs, rhs, temp2);
    }
    
    bool ret = !element_cmp(lhs, rhs);
    element_clear(lhs);
    element_clear(rhs);
    if (ret) {
        cout<<"Verfied"<<endl;
    } else {
        cout<<"Signature rejected"<<endl;
    }
    return ret;
}

element_t* PKG::getst() {
    return this->st;
}
element_t* PKG::getSt() {
    return this->St;
}
element_t* PKG::getPt() {
    return this->Pt;
}
vector<element_t*> PKG::getQt() {
    return this->Qt;
}

string PKG::getID_1ToID_t(int t,string id) {
    string s = id;
    int count = std::count (s.begin(), s.end(), ',');
    while (count+1 > t) {
        size_t pos = s.rfind(",");
        s.erase(pos,s.length());
        count--;
    }
    return s;
}

PKGTree::PKGTree() {
    treeHeight = 0;
    root = new PKG();
    root->rootSetUp();
    lookUp.insert({"", root});
}

PKGTree::~PKGTree() {
    delete root;
}

void PKGTree::insert(string id, string newId) {
    PKG *node = lookUpNode(id);
    PKG *newNode = new PKG();
    
    // add newNode to one of node's children
    node -> children.push_back(newNode);
    
    // set node to newNode's parent
    newNode -> parent = node;
    
    // set node's height, idVector and idString
    newNode -> height = node -> height + 1;
    newNode -> idString = node -> idString;
    newNode -> idAppend(newId);
    
    newNode -> idVector = node -> idVector;
    newNode -> idVector.push_back(newId);
    lookUp.insert({newNode -> idString, newNode});
    newNode->lowerLevelSetUp();
}

void PKGTree::printTree() {
    root -> printID();
    printTreeHelper(root);
}

void PKGTree::printTreeHelper(PKG* node) {
    if (node -> children.empty()) return;
    for (PKG *child : node -> children) {
        child -> printID();
        printTreeHelper(child);
    }
}

PKG* PKGTree::lookUpNode(string id) {
    unordered_map<string,PKG*>::const_iterator got = lookUp.find(id);
    if (got == lookUp.end()) {
        cout<<"No node with idString: "<<id<<endl;
        return NULL;
    }
    return got -> second;
}



    





