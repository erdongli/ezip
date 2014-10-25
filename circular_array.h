#ifndef CIRCULAR_ARRAY_H
#define CIRCULAR_ARRAY_H

class CircularArray
{
public:
    CircularArray(unsigned int c);
    ~CircularArray();

    unsigned int getSize() const;
    unsigned int getCapacity() const;
    bool empty() const;

    char at(unsigned int index) const;

    void push(char c);
    void shrink();

private:
    unsigned int convert(unsigned int index) const;

    unsigned int head;
    unsigned int size;
    unsigned int capacity;
    char *items;
};

#endif
