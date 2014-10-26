#include "compressor.h"
#include "bitstream.h"
#include <fstream>

const char kUncoded = 0x0;
const char kEncoded = 0x1;
const unsigned int kWindowSize = (1 << 16) - 1;
const unsigned int kMaxUncoded = 2;
const unsigned int kMaxEncoded = (1 << 6) + kMaxUncoded;
const unsigned int kRoot = kWindowSize + 1;
const unsigned int kUnused = kWindowSize;

using std::fstream; using std::ifstream;
using std::string;
using std::vector;

Compressor::Compressor(const string &i, const string &o)
    : windowHead(0), ifname(i), ofname(o), lookAheadBuffer(kMaxEncoded),
      slidingWindow(kWindowSize, 0), roots(256, kUnused),
      parents(kWindowSize, kUnused), lefts(kWindowSize, kUnused),
      rights(kWindowSize, kUnused)
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
    unsigned int offset = roots[static_cast<unsigned char>(lookAheadBuffer.at(0))];

    while (result.length < kMaxEncoded && offset != kUnused) {
        unsigned int diff = 0;
        unsigned int length = 1;

        while (length < lookAheadBuffer.getSize() &&
                !(diff = slidingWindow[(offset+length)%kWindowSize]-lookAheadBuffer.at(length)) &&
                ++length < kMaxEncoded);

        if (length > result.length) {
            result.offset = offset;
            result.length = length;
        }

        if (diff > 0) {
            offset = lefts[offset];
        }
        else {
            offset = rights[offset];
        }
    }

    return result;
}

void Compressor::update()
{
    unsigned int index;

    if (windowHead < kMaxUncoded) {
        index = windowHead + kWindowSize - kMaxUncoded;
    }
    else {
        index = windowHead - kMaxUncoded;
    }

    for (int len = 0; len < kMaxUncoded+1; len++) {
        erase((index+len)%kWindowSize);
    }

    slidingWindow[windowHead] = lookAheadBuffer.at(0);

    for (int len = 0; len < kMaxUncoded+1; len++) {
        insert((index+len)%kWindowSize);
    }

    windowHead = (windowHead + 1) % kWindowSize;
}

void Compressor::insert(unsigned int index)
{
    lefts[index] = kUnused;
    rights[index] = kUnused;

    unsigned int curr = roots[static_cast<unsigned char>(slidingWindow[index])];

    if (curr == kUnused) {
        parents[index] = kRoot;
        roots[static_cast<unsigned char>(slidingWindow[index])] = index;

        return;
    }

    unsigned int diff = compare(index, curr);

    if (!diff) {
        parents[index] = kRoot;
        lefts[index] = lefts[curr];
        rights[index] = rights[curr];
        fix(index);

        parents[curr] = kUnused;
        lefts[curr] = kUnused;
        rights[curr] = kUnused;

        roots[static_cast<unsigned char>(slidingWindow[index])] = index;

        return;
    }

    while (curr != kUnused) {
        int diff = compare(index, curr);

        if (diff < 0) {
            if (lefts[curr] != kUnused) {
                curr = lefts[curr];
                continue;
            }

            // arrives at leave
            lefts[curr] = index;
            parents[index] = curr;
            fix(index);
            return;
        }

        if (diff > 0) {
            if (rights[curr] != kUnused) {
                curr = rights[curr];
                continue;
            }

            // arrives at leave
            rights[curr] = index;
            parents[index] = curr;
            fix(index);
            return;
        }

        // replace
        parents[index] = parents[curr];
        lefts[index] = lefts[curr];
        rights[index] = rights[curr];
        fix(index);

        if (lefts[parents[curr]] == curr) {
            lefts[parents[curr]] = index;
        }
        else {
            rights[parents[curr]] = index;
        }

        parents[curr] = kUnused;
        lefts[curr] = kUnused;
        rights[curr] = kUnused;

        return;
    }
}

void Compressor::erase(unsigned int index)
{
    if (parents[index] == kUnused) {
        return;
    }

    unsigned int curr;

    if (lefts[index] == kUnused) {
        curr = rights[index];
    }
    else if (rights[index] == kUnused) {
        curr = lefts[index];
    }
    else {
        // find pred
        curr = lefts[index];

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


    if (parents[index] == kRoot) {
        roots[static_cast<unsigned char>(slidingWindow[index])] = curr;
    } else {
        if (lefts[parents[index]] == index) {
            lefts[parents[index]] = curr;
        }
        else {
            rights[parents[index]] = curr;
        }
    }

    parents[curr] = parents[index];

    parents[index] = kUnused;
    lefts[index] = kUnused;
    rights[index] = kUnused;
}

void Compressor::fix(unsigned int index)
{
    if (lefts[index] != kWindowSize) {
        parents[lefts[index]] = index;
    }

    if (rights[index] != kWindowSize) {
        parents[rights[index]] = index;
    }
}

int Compressor::compare(unsigned int index0, unsigned int index1)
{
    unsigned int offset;
    int result = 0;

    for (offset = 0; offset < kMaxEncoded; offset++) {
        result = slidingWindow[(index0+offset)%kWindowSize] - slidingWindow[(index1+offset)%kWindowSize];
        if (result) {
            break;
        }
    }

    return result;
}
