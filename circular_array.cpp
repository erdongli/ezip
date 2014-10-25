#include "circular_array.h"

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

void CircularArray::shrink()
{
    head = convert(1);
    size--;
}

unsigned int CircularArray::convert(unsigned int index) const
{
    return (head + index) % capacity;
}
