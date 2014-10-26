#include "decompressor.h"
#include "bitstream.h"
#include <fstream>

const char kUncoded = 0x0;
const unsigned int kWindowSize = (1 << 16) - 1;
const unsigned int kMaxUncoded = 2;
const unsigned int kMaxEncoded = (1 << 6) + kMaxUncoded;

using std::fstream; using std::ofstream;
using std::string;

Decompressor::Decompressor(const string &i, const string &o)
    : windowHead(0), ifname(i), ofname(o), slidingWindow(kWindowSize, 0)
{
    // do nothing
}

void Decompressor::decompress()
{
    BitStream is(ifname, fstream::in);
    ofstream os(ofname);
    char *buffer = new char[kMaxEncoded]();

    while (is.good()) {
        char c = is.getb();
        if (!is.good()) {
            break;
        }

        if (c == kUncoded) {
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

        unsigned int length = is.get6b() + kMaxUncoded + 1;
        if (!is.good()) {
            break;
        }

        for (int i = 0; i < length; i++) {
            buffer[i] = slidingWindow[(offset+i)%kWindowSize];
        }
        for (int i = 0; i < length; i++) {
            slidingWindow[windowHead] = buffer[i];
            windowHead = (windowHead + 1) % kWindowSize;
        }
        os.write(buffer, length);
    }

    delete[] buffer;
    os.close();
    is.close();
}
