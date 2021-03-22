//
//  HIDParameter.hpp
//  HIBE
//
//  Created by Titus Wong on 18/8/2020.
//  Copyright © 2020 Titus Wong. All rights reserved.
//

#ifndef HIBEParameter_hpp
#define HIBEParameter_hpp
#include <iomanip>
#include <sstream>
#include <fstream>
#include <pbc.h>
#include <string>
#include <tuple>

using namespace std;

class HIDParameter
{
    private:
    int sizeG1,sizeGT;
    mpz_t *q; // from param
    element_t* s0; //private key
    element_t* P0;
    element_t* Q0;

    public:
    const int messageSpace = 800; //1 kb
    pairing_t *pairing;
    HIDParameter();
    ~HIDParameter();
    mpz_t* getQ();
    mpz_t* getP();
    element_t* getE();
    element_t* getP0();
    element_t* getQ0();
    element_t* gets0();
    element_t* H1(string s);
    string H2(element_t e);
    element_t* H3(string s1, string s2);
    string H4(string s);
    element_t* H5(string s);
    string XOR(string message, string padding);
    
};

#endif /* HIBEParameter_hpp */
