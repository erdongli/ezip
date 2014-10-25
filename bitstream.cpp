#include "bitstream.h"

using std::ios_base;
using std::string;

const unsigned int kBitsPerByte = 8;

BitStream::BitStream(const string &filename, ios_base::openmode mode)
    : buffer(0), count(0), fs(filename, mode)
{
    // do nothing
}

BitStream::~BitStream()
{
    if (fs.is_open()) {
        flush();
        fs.close();
    }
}

bool BitStream::good() const
{
    return fs.good();
}

void BitStream::close()
{
    flush();
    fs.close();
}

char BitStream::getc()
{
    char c;

    fs.get(c);
    if (good() && count) {
        unsigned char tmp = static_cast<unsigned char>(c) >> count;

        tmp |= buffer << (kBitsPerByte - count);
        buffer = c;
        c = tmp;
    }

    return c;
}

void BitStream::putc(char c)
{
    if (count) {
        unsigned char tmp = static_cast<unsigned char>(c) >> count;

        tmp |= buffer << (kBitsPerByte - count);
        buffer = c;
        c = tmp;
    }

    fs.put(c);
}

char BitStream::getb()
{
    if (!count) {
        fs.get(buffer);
        count = kBitsPerByte;
    }

    return (buffer >> --count) & 0x1;
}

void BitStream::putb(char b)
{
    buffer = (buffer << 1) | (b & 0x1);
    count++;

    if (count == kBitsPerByte) {
        fs.put(buffer);
        buffer = 0;
        count = 0;
    }
}

uint8_t BitStream::get6b()
{
    uint8_t bits = 0;
    unsigned int nRemain = 6;

    while (nRemain--) {
        bits = (bits << 1) | (getb() & 0x1);
        if (!good()) {
            break;
        }
    }

    return bits;
}

void BitStream::put6b(uint8_t bits)
{
    unsigned int nRemain = 6;

    while (nRemain--) {
        putb((bits >> nRemain) & 0x1);
    }
}

uint16_t BitStream::get16b()
{
    uint16_t bits;
    char *bp = reinterpret_cast<char *>(&bits);

    bp[0] = getc();
    if (!good()) {
        return bits;
    }

    bp[1] = getc();

    return bits;
}

void BitStream::put16b(uint16_t bits)
{
    char *bp = reinterpret_cast<char *>(&bits);

    putc(bp[0]);
    putc(bp[1]);
}

void BitStream::flush()
{
    if (count) {
        putc(buffer << (kBitsPerByte - count));
    }

    buffer = 0;
    count = 0;
}
