#ifndef DECOMPRESSOR_H
#define DECOMPRESSOR_H

#include "bit_file.h"
#include "circular_array.h"
#include <string>

class Decompressor {
public:
    Decompressor(const std::string &ifname, const std::string &ofname);

    void decompress();

private:
    std::string ifname;
    std::string ofname;
    CircularArray slidingWindow;
};

#endif
