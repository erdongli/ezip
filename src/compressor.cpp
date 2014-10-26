#include "compressor.h"
#include "bitstream.h"
#include "constants.h"
#include <fstream>

const unsigned int kRootFlag = kWindowSize;
const unsigned int kUnused = kRootFlag + 1;

using std::fstream; using std::ifstream;
using std::string;

Compressor::Compressor(const string &i, const string &o)
    : windowHead(0), bufferHead(0), bufferSize(0),
      ifname(i), ofname(o),
      slidingWindow(kWindowSize, 0),
      lookAheadBuffer(kMaxEncodeLength, 0),
      roots(kNumAsciiChars, kUnused), parents(kWindowSize, kUnused),
      lefts(kWindowSize, kUnused), rights(kWindowSize, kUnused)
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

    // pre-fill look-ahead buffer
    while (is.good() && bufferSize < kMaxEncodeLength) {
        char c = is.get();

        if (is.good()) {
            lookAheadBuffer[bufferSize++] = c;
        }
    }

    while (bufferSize) {
        MatchResult result = match();

        if (result.length <= kMaxUncodeLength) {
            // uncode
            os.putb(kUncodedFlag);
            os.putc(lookAheadBuffer[bufferHead]);
            result.length = 1;
        }
        else {
            // encode
            os.putb(kEncodedFlag);
            os.put16b(static_cast<uint16_t>(result.offset));
            os.put6b(static_cast<uint8_t>(result.length-kMaxUncodeLength-1));
        }

        while (result.length--) {
            updateWindow();

            // update look-ahead buffer
            char c = is.get();

            lookAheadBuffer[bufferHead] = c;
            bufferHead = (bufferHead + 1) % kMaxEncodeLength;
            bufferSize -= is.good() ? 0 : 1;
        }
    }

    os.close();
    is.close();
}

Compressor::MatchResult Compressor::match() const
{
    unsigned int offset;
    MatchResult result;

    offset = roots[static_cast<unsigned char>(lookAheadBuffer[bufferHead])];

    while (result.length < kMaxEncodeLength && offset != kUnused) {
        unsigned int diff = 0;
        unsigned int length = 1;

        while (length < bufferSize &&
                !(diff = slidingWindow[(offset+length)%kWindowSize]
                    -lookAheadBuffer[(bufferHead+length)%kMaxEncodeLength]) &&
                ++length < kMaxEncodeLength);

        if (length > result.length) {
            result.offset = offset;
            result.length = length;
        }

        offset = diff > 0 ? lefts[offset] : rights[offset];
    }

    return result;
}

void Compressor::updateWindow()
{
    unsigned int index = windowHead + kWindowSize - kMaxUncodeLength;

    // erase affected data
    for (int len = 0; len < kMaxUncodeLength+1; len++) {
        erase((index+len)%kWindowSize);
    }

    slidingWindow[windowHead] = lookAheadBuffer[bufferHead];

    // re-insert affected data
    for (int len = 0; len < kMaxUncodeLength+1; len++) {
        insert((index+len)%kWindowSize);
    }

    windowHead = (windowHead + 1) % kWindowSize;
}

void Compressor::insert(unsigned int index)
{
    lefts[index] = kUnused;
    rights[index] = kUnused;

    unsigned int curr = roots[static_cast<unsigned char>(slidingWindow[index])];

    // replace root
    if (curr == kUnused || !compare(index, curr)) {
        if (!kUnused) {
            lefts[index] = lefts[curr];
            rights[index] = rights[curr];

            fixChildren(index);

            parents[curr] = kUnused;
        }

        parents[index] = kRootFlag;
        roots[static_cast<unsigned char>(slidingWindow[index])] = index;

        return;
    }

    while (curr != kUnused) {
        int diff = compare(index, curr);

        if (diff < 0) {
            // branch left
            if (lefts[curr] != kUnused) {
                curr = lefts[curr];
                continue;
            }

            // arrives at leave
            lefts[curr] = index;
            parents[index] = curr;
            fixChildren(index);
            return;
        }

        if (diff > 0) {
            // branch right
            if (rights[curr] != kUnused) {
                curr = rights[curr];
                continue;
            }

            // arrives at leave
            rights[curr] = index;
            parents[index] = curr;
            fixChildren(index);
            return;
        }

        // replace
        lefts[index] = lefts[curr];
        rights[index] = rights[curr];

        fixParent(index, curr);
        fixChildren(index);

        return;
    }
}

void Compressor::erase(unsigned int index)
{
    if (parents[index] == kUnused) {
        // data is not indexed
        return;
    }

    unsigned int curr = lefts[index];

    if (lefts[index] == kUnused) {
        curr = rights[index];
    }
    else if (rights[index] != kUnused) {
        // find pred
        while (rights[curr] != kUnused) {
            curr = rights[curr];
        }

        if (curr != lefts[index]) {
            rights[parents[curr]] = lefts[curr];
            parents[lefts[curr]] = parents[curr];
            lefts[curr] = lefts[index];
            parents[lefts[index]] = curr;
        }

        rights[curr] = rights[index];
        parents[rights[index]] = curr;
    }


    fixParent(curr, index);
}

void Compressor::fixParent(unsigned int index, unsigned int origin)
{
    if (parents[origin] == kRootFlag) {
        roots[static_cast<unsigned char>(slidingWindow[origin])] = index;
    } else {
        if (lefts[parents[origin]] == origin) {
            lefts[parents[origin]] = index;
        }
        else {
            rights[parents[origin]] = index;
        }
    }

    parents[index] = parents[origin];
    parents[origin] = kUnused;
}

void Compressor::fixChildren(unsigned int index)
{
    // fix left child
    if (lefts[index] != kWindowSize) {
        parents[lefts[index]] = index;
    }

    // fix right child
    if (rights[index] != kWindowSize) {
        parents[rights[index]] = index;
    }
}

int Compressor::compare(unsigned int index0, unsigned int index1)
{
    // compare characters one by one
    for (unsigned int i = 0; i < kMaxEncodeLength; i++) {
        int diff = slidingWindow[(index0+i)%kWindowSize]
            - slidingWindow[(index1+i)%kWindowSize];
        if (diff) {
            return diff;
        }
    }

    return 0;
}
