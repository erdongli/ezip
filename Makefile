CC=g++
CFLAGS=-c -Wall

all: ezip

ezip: ezip.o bitstream.o compressor.o decompressor.o
	$(CC) ezip.o bitstream.o compressor.o decompressor.o -o ezip
	rm -rf *.o

ezip.o: src/ezip.cpp
	$(CC) $(CFLAGS) src/ezip.cpp

bitstream.o: src/bitstream.cpp
	$(CC) $(CFLAGS) src/bitstream.cpp

compressor.o: src/compressor.cpp
	$(CC) $(CFLAGS) src/compressor.cpp

decompressor.o: src/decompressor.cpp
	$(CC) $(CFLAGS) src/decompressor.cpp

clean:
	rm -rf *.o ezip
