#include "decompressor.h"
#include "bitstream.h"
#include "constants.h"
#include <fstream>

using std::fstream; using std::ofstream;
using std::string;
using std::vector;

Decompressor::Decompressor(const string &i, const string &o)
    : windowHead(0), ifname(i), ofname(o), slidingWindow(kWindowSize, 0)
{
    // do nothing
}

void Decompressor::decompress()
{
    BitStream is(ifname, fstream::in);
    ofstream os(ofname);
    vector<char> buffer(kMaxEncodeLength, 0);

    while (is.good()) {
        char c = is.getb();
        if (!is.good()) {
            break;
        }

        if (c == kUncodedFlag) {
            // directly put character to bitstream
            c = is.getc();
            if (!is.good()) {
                break;
            }

            os.put(c);
            slidingWindow[windowHead] = c;
            windowHead = (windowHead + 1) % kWindowSize;
            continue;
        }

        unsigned int offset = is.get16b();
        if (!is.good()) {
            break;
        }

        unsigned int length = is.get6b() + kMaxUncodeLength + 1;
        if (!is.good()) {
            break;
        }

        // fill up temp buffer with data
        for (unsigned int i = 0; i < length; i++) {
            buffer[i] = slidingWindow[(offset+i)%kWindowSize];
        }

        // append sliding window with data stored in temp buffer
        for (unsigned int i = 0; i < length; i++) {
            slidingWindow[windowHead] = buffer[i];
            windowHead = (windowHead + 1) % kWindowSize;
        }

        // write out to ofstream
        os.write(buffer.data(), length);
    }

    os.close();
    is.close();
}
