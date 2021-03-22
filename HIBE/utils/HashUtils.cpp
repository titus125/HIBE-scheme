#include "HashUtils.hpp"


using namespace lrs;

string HashUtils::calcHash(string value, int bits, bool decodeInput, bool encodeOutput)
{
  
  SHAKE128 shake128(bits);
  string digest;

  CryptoPP::StringSource s(
      decodeInput ? HexUtils::decode(value) : value,
      true,
      new CryptoPP::HashFilter(shake128, HexUtils::getHexEncoder(digest)));

  return encodeOutput ? digest : HexUtils::decode(digest);
}
