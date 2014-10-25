#include "circular_array.h"
#include <algorithm>
#include <cstdlib>

using std::min;

CircularArray::CircularArray(unsigned int c)
    : head(0), size(0), capacity(c)
{
    items = new char[capacity]();
}

CircularArray::~CircularArray()
{
    delete[] items;
}

unsigned int CircularArray::getSize() const
{
    return size;
}

unsigned int CircularArray::getCapacity() const
{
    return capacity;
}

bool CircularArray::empty() const
{
    return !size;
}

char CircularArray::at(unsigned int index) const
{
    return items[convert(index)];
}

void CircularArray::push(char c)
{
    if (size < capacity) {
        items[convert(size++)] = c;
        return;
    }

    items[head] = c;
    head = convert(1);
}

void CircularArray::pushn(const char *src, unsigned int len)
{
    for (int i = 0; i < len; i++) {
        push(src[i]);
    }
}

void CircularArray::shrink()
{
    head = convert(1);
    size--;
}

void CircularArray::copy(char *dest, unsigned int index, unsigned int len) const
{
    index = convert(index);
    memcpy(dest, items+index, min(len, capacity-index));
    if (capacity - index < len) {
        memcpy(dest+capacity-index, items, len-(capacity-index));
    }
}

unsigned int CircularArray::convert(unsigned int index) const
{
    return (head + index) % capacity;
}
