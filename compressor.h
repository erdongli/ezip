#ifndef COMPRESSOR_H
#define COMPRESSOR_H

#include "bit_file.h"
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

        MatchResult(unsigned int o, unsigned int l)
            : offset(o), length(l)
        {}
    };

    MatchResult match() const;
    void update();

    std::string ifname;
    std::string ofname;
    CircularArray slidingWindow;
    CircularArray lookAheadBuffer;
};

#endif
