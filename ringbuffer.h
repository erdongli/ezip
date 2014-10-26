#ifndef RINGBUFFER_H
#define RINGBUFFER_H

class RingBuffer
{
public:
    RingBuffer(unsigned int c);
    ~RingBuffer();

    unsigned int getSize() const;
    bool empty() const;

    char operator[](unsigned int index) const;
    char& operator[](unsigned int index);

    void push(char c);
    void pushn(const char *src, unsigned int len);
    void shrink();

    void copy(char *dest, unsigned int index, unsigned int len) const;

private:
    unsigned int convert(unsigned int index) const;

    unsigned int head;
    unsigned int size;
    unsigned int capacity;
    char *items;
};

#endif
