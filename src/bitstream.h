#ifndef BITSTREAM_H
#define BITSTREAM_H

#include <cstdint>
#include <fstream>
#include <string>

/*
 * stream class to operate on bits
 *
 * author: erdong li
 * date: october 25, 2014
 */
class BitStream {
public:
    BitStream(const std::string &filename,
              std::ios_base::openmode mode = std::ios_base::in | std::ios_base::out);
    ~BitStream();

    /*
     * wrapper function on fstream.good()
     */
    bool good() const;
    /*
     * flush remaining bits and close fstream
     */
    void close();

    /*
     * get/put a sincle char from/to fstream
     */
    char getc();
    void putc(char c);

    /*
     * get/put a single bit from/to fstream
     */
    char getb();
    void putb(char b);

    /*
     * get/put 6 bits from/to fstream
     */
    uint8_t get6b();
    void put6b(uint8_t bits);

    /*
     * get/put 16 bits from/to fstream
     */
    uint16_t get16b();
    void put16b(uint16_t bits);

private:
    /*
     * flush remaining bits to fstream
     */
    void flush();

    char buffer;
    uint8_t count;
    std::fstream fs;
};

#endif
