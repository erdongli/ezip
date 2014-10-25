#include "decompressor.h"
#include "bit_file.h"
#include <fstream>

const char kUncoded = 0x0;
const unsigned int kWindowSize = (1 << 16) - 1;
const unsigned int kMaxUncoded = 2;
const unsigned int kMaxEncoded = (1 << 6) + kMaxUncoded;

using std::fstream; using std::ofstream;
using std::string;

Decompressor::Decompressor(const string &i, const string &o)
    : ifname(i), ofname(o), slidingWindow(kWindowSize)
{
    // do nothing
}

void Decompressor::decompress()
{
    BitFile infile(ifname, fstream::in);
    ofstream outfile(ofname);
    char *buffer = new char[kMaxEncoded]();

    while (infile.good()) {
        char c = infile.getb();
        if (!infile.good()) {
            break;
        }

        if (c == kUncoded) {
            c = infile.getc();
            if (!infile.good()) {
                break;
            }

            outfile.put(c);
            slidingWindow.push(c);
            continue;
        }

        unsigned int offset = infile.get16b();
        if (!infile.good()) {
            break;
        }

        unsigned int length = infile.get6b() + kMaxUncoded + 1;
        if (!infile.good()) {
            break;
        }

        slidingWindow.copy(buffer, offset, length);
        /*
        for (int i = 0; i < length; i++) {
            //outfile.put(c);
            buffer[i] = slidingWindow.at(offset+i);
        }
        */
        slidingWindow.pushn(buffer, length);
        outfile.write(buffer, length);
    }

    delete[] buffer;
    outfile.close();
    infile.close();
}
