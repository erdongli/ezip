#ifndef BITSTREAM_H
#define BITSTREAM_H

#include <cstdint>
#include <fstream>
#include <string>

class BitStream {
public:
    BitStream(const std::string &filename,
              std::ios_base::openmode mode = std::ios_base::in | std::ios_base::out);
    ~BitStream();

    bool good() const;
    void close();

    char getc();
    void putc(char c);

    char getb();
    void putb(char b);

    uint8_t get6b();
    void put6b(uint8_t bits);

    uint16_t get16b();
    void put16b(uint16_t bits);

private:
    void flush();
    bool isLittleEndian() const;

    char buffer;
    uint8_t count;
    std::fstream fs;
};

#endif
