#ifndef DECOMPRESSOR_H
#define DECOMPRESSOR_H

#include "circular_array.h"
#include <string>

class Decompressor {
public:
    Decompressor(const std::string &i, const std::string &o);

    void decompress();

private:
    std::string ifname;
    std::string ofname;
    CircularArray slidingWindow;
};

#endif
