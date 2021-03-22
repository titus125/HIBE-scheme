//
//  HIDParameter.cpp
//  HIBE
//
//  Created by Titus Wong on 18/8/2020.
//  Copyright © 2020 Titus Wong. All rights reserved.
//

#include "HIDParameter.hpp"
#include <iostream>
#include <stdio.h>
#include "./utils/HashUtils.hpp"

using namespace lrs;

HIDParameter::HIDParameter(){
    pairing = new pairing_t[1];
    FILE *file = fopen("HIBE/a.param", "r");
    char buf[4096];
    fread(buf, 1, 4096, file);

    pairing_init_set_str(*pairing, (const char*) buf);
    
    this->P0 = new element_t[1];
    element_init_G1(*P0, *pairing);
    element_random(*P0);
    
    //setup q, which is the order of the group
    // We do this by doing 0-1 (mod q) which would give us q-1
    element_t tempZrOne;
    element_init_Zr(tempZrOne,*(pairing));
    element_set0(tempZrOne);


    element_t tempZrZero;
    element_init_Zr(tempZrZero,*(pairing));
    element_set1(tempZrZero);

    element_sub(tempZrOne,tempZrOne,tempZrZero);

    this->q = new mpz_t[1];

    mpz_t tempP;
    mpz_init(tempP);
    element_to_mpz(tempP,tempZrOne);

    mpz_init(*(this->q));
    mpz_set(*(this->q),tempP);
    element_clear(tempZrOne);
    element_clear(tempZrZero);
    mpz_clear(tempP);
    
    //add 1 to get the ring size
    mpz_add_ui(*(this->q), *(this->q), 1);
    //gmp_printf ("%s is an mpz %Zd\n", "here", *q);
    
    this->sizeG1 = pairing_length_in_bytes_G1(*(pairing));
    this->sizeGT = pairing_length_in_bytes_GT(*(pairing));
    
    //setup s0
    this->s0 = new element_t[1];
    element_init_Zr(*(this->s0),*(this->pairing));
    element_random(*(this->s0));
    
    //Q0 = s0*P0
    this->Q0 = new element_t[1];
    element_init_G1(*(this->Q0), *pairing);
    element_mul_zn(*(this->Q0), *(this->P0), *(this->s0));
}

HIDParameter::~HIDParameter() {
    mpz_clear(*q);
    element_clear(*P0);
    element_clear(*Q0);
    pairing_clear(*pairing);
    delete q,P0,Q0,pairing;
}

element_t* HIDParameter::H1(string s){
    string result = HashUtils::calcHash(s,this->sizeG1);
    int n = result.length();
    char char_array[n + 1];
    strcpy(char_array, result.c_str());

    element_t *ret = new element_t[1];
    element_init_G1(*ret,*(this->pairing));
    element_from_hash(*ret, char_array, n*16);

    return ret;
}

string HIDParameter::H2(element_t e) {
    mpz_t temp;
    mpz_init(temp);
    element_to_mpz(temp,e);
    
    char * str = mpz_get_str(NULL,16,temp);
    mpz_init_set_str(temp, str, 16);
    
    string result = HashUtils::calcHash(str,messageSpace);

    delete[] str;
    return result;
}

element_t* HIDParameter::H3(string s1, string s2) {
    string result = HashUtils::calcHash(XOR(s1,s2),messageSpace);
    int n = result.length();
    char char_array[n + 1];
    strcpy(char_array, result.c_str());

    element_t *ret = new element_t[1];
    element_init_Zr(*ret,*(this->pairing));
    element_from_hash(*ret, char_array, n*16);

    return ret;
}

string HIDParameter::H4(string s) {
    return HashUtils::calcHash(s,messageSpace);
}

element_t* HIDParameter::H5(string s) {
    return H1(HashUtils::calcHash(s,messageSpace));
}

element_t* HIDParameter::getP0(){
    return this->P0;
}

element_t* HIDParameter::getQ0(){
    return this->Q0;
}

element_t* HIDParameter::gets0(){
    return this->s0;
}


string HIDParameter::XOR(string message, string padding)
{
    string ret(message);

    short unsigned int klen=padding.length();
    short unsigned int vlen=message.length();
    short unsigned int k=0;
    short unsigned int v=0;

    for(v;v<vlen;v++)
    {
        ret[v]=message[v]^padding[k];
        k=(++k<klen?k:0);
    }

    return ret;
}


