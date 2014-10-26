#ifndef CONSTANTS_H
#define CONSTANTS_H

const char kUncodedFlag = 0x0;
const char kEncodedFlag = 0x1;
const unsigned int kWindowSize = 1 << 16;
const unsigned int kMaxUncodeLength = 2;
const unsigned int kMaxEncodeLength = (1 << 6) + kMaxUncodeLength;
const unsigned int kBitsPerByte = 8;
const unsigned int kNumAsciiChars = 1 << kBitsPerByte;

#endif
