#ifndef COMPRESSOR_H
#define COMPRESSOR_H

#include "ringbuffer.h"
#include <string>
#include <vector>

/*
 * compress data from i and write to o
 *
 * author: erdong li
 * date: october 25, 2014
 */
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

    // find match
    MatchResult match() const;
    void updateWindow();

    // tree manipulations
    void insert(unsigned int index);
    void erase(unsigned int index);

    // helper functions
    void fixParent(unsigned int index, unsigned int origin);
    void fixChildren(unsigned int index);
    int compare(unsigned int index0, unsigned int index1);

    unsigned int windowHead;
    std::string ifname;
    std::string ofname;
    std::vector<char> slidingWindow;
    RingBuffer lookAheadBuffer;
    std::vector<unsigned int> roots;
    std::vector<unsigned int> parents;
    std::vector<unsigned int> lefts;
    std::vector<unsigned int> rights;
};

#endif
