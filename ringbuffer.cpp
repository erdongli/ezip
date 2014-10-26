#include "ringbuffer.h"
#include <algorithm>
#include <cstdlib>

using std::min;

RingBuffer::RingBuffer(unsigned int c)
    : head(0), size(0), capacity(c)
{
    items = new char[capacity]();
}

RingBuffer::~RingBuffer()
{
    delete[] items;
}

unsigned int RingBuffer::getSize() const
{
    return size;
}

bool RingBuffer::empty() const
{
    return !size;
}

char RingBuffer::operator[](unsigned int index) const
{
    return items[convert(index)];
}

char& RingBuffer::operator[](unsigned int index)
{
    return items[convert(index)];
}

void RingBuffer::push(char c)
{
    if (size < capacity) {
        items[convert(size++)] = c;
        return;
    }

    items[head] = c;
    head = convert(1);
}

void RingBuffer::pushn(const char *src, unsigned int len)
{
    for (int i = 0; i < len; i++) {
        push(src[i]);
    }
}

void RingBuffer::shrink()
{
    head = convert(1);
    size--;
}

void RingBuffer::copy(char *dest, unsigned int index, unsigned int len) const
{
    index = convert(index);
    memcpy(dest, items+index, min(len, capacity-index));
    if (capacity - index < len) {
        memcpy(dest+capacity-index, items, len-(capacity-index));
    }
}

unsigned int RingBuffer::convert(unsigned int index) const
{
    return (head + index) % capacity;
}
