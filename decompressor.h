#ifndef DECOMPRESSOR_H
#define DECOMPRESSOR_H

#include <string>
#include <vector>

/*
 * decompress data from i and write to o
 *
 * author: erdong li
 * date: october 25, 2014
 */
class Decompressor {
public:
    Decompressor(const std::string &i, const std::string &o);

    void decompress();

private:
    unsigned int windowHead;
    std::string ifname;
    std::string ofname;
    std::vector<char> slidingWindow;
};

#endif
