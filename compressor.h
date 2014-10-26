#ifndef COMPRESSOR_H
#define COMPRESSOR_H

#include "ringbuffer.h"
#include <string>
#include <vector>

class Compressor {
public:
    Compressor(const std::string &i, const std::string &o);

    void compress();

private:
    struct MatchResult {
        unsigned int offset;
        unsigned int length;

        MatchResult();
    };

    MatchResult match() const;
    void update();
    void insert(unsigned int index);
    void erase(unsigned int index);
    void fix(unsigned int index);
    int compare(unsigned int index0, unsigned int index1);

    unsigned int windowHead;
    std::string ifname;
    std::string ofname;
    RingBuffer lookAheadBuffer;
    std::vector<char> slidingWindow;
    std::vector<unsigned int> roots;
    std::vector<unsigned int> parents;
    std::vector<unsigned int> lefts;
    std::vector<unsigned int> rights;
};

#endif
