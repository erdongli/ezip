#include "compressor.h"
#include "bitstream.h"
#include <fstream>

const char kUncoded = 0x0;
const char kEncoded = 0x1;
const unsigned int kWindowSize = (1 << 16) - 1;
const unsigned int kMaxUncoded = 2;
const unsigned int kMaxEncoded = (1 << 6) + kMaxUncoded;

using std::fstream; using std::ifstream;
using std::string;

Compressor::Compressor(const string &i, const string &o)
    : ifname(i), ofname(o), slidingWindow(kWindowSize), lookAheadBuffer(kMaxEncoded)
{
    // do nothing
}

Compressor::MatchResult::MatchResult()
    : offset(0), length(0)
{
    // do nothing
}

void Compressor::compress()
{
    ifstream is(ifname);
    BitStream os(ofname, fstream::out);

    while (is.good() && lookAheadBuffer.getSize() < lookAheadBuffer.getCapacity()) {
        char c = is.get();

        if (is.good()) {
            lookAheadBuffer.push(c);
        }
    }

    while (!lookAheadBuffer.empty()) {
        MatchResult result = match();

        if (result.length <= kMaxUncoded) {
            os.putb(kUncoded);
            os.putc(lookAheadBuffer.at(0));
            result.length = 1;
        }
        else {
            os.putb(kEncoded);
            os.put16b(static_cast<uint16_t>(result.offset));
            os.put6b(static_cast<uint8_t>(result.length-kMaxUncoded-1));
        }

        while (result.length--) {
            char c = is.get();

            update();

            if (is.good()) {
                lookAheadBuffer.push(c);
            }
            else {
                lookAheadBuffer.shrink();
            }
        }
    }

    os.close();
    is.close();
}

Compressor::MatchResult Compressor::match() const
{
    MatchResult result;
    unsigned int offset = 0;
    unsigned int length = 1;

    while (result.length < kMaxEncoded && offset < slidingWindow.getCapacity()) {
        if (slidingWindow.at(offset) == lookAheadBuffer.at(0)) {
            length = 1;

            while (length < lookAheadBuffer.getSize() &&
                    slidingWindow.at(offset+length) == lookAheadBuffer.at(length) &&
                    ++length < kMaxEncoded);

            if (length > result.length) {
                result.offset = offset;
                result.length = length;
            }
        }

        offset++;
    }

    return result;
}

void Compressor::update()
{
    slidingWindow.push(lookAheadBuffer.at(0));
}
