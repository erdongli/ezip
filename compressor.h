#ifndef COMPRESSOR_H
#define COMPRESSOR_H

#include "circular_array.h"
#include <string>

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

    std::string ifname;
    std::string ofname;
    CircularArray slidingWindow;
    CircularArray lookAheadBuffer;
};

#endif
