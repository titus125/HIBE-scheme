#pragma once

#include <string>
#include <iostream>
#include "HexUtils.hpp"

#include "./cryptopp/shake.h"
#include "./cryptopp/hex.h"
//#include "../../build/libraries/cryptopp/sha.h"
//#include "../../build/libraries/cryptopp/hex.h"

using namespace std;
using CryptoPP::byte;
using CryptoPP::SHAKE128;

namespace lrs
{

class HashUtils
{
private:
  //static SHAKE128 shake128;

public:
  static string calcHash(string value, int bits,bool decodeInput = false, bool encodeOutput = true);
};

}
