# EZip

EZip is a compression/decompression utility implemented with LZSS algorithm in C++.

## Usage

To build the facility:

        make

Compress a file:

        ./ezip -i <input file name> -o <output file name>

Decompress a file:

        ./ezip -d -i <input file name> -o <output file name>

## Implementation

### Compressor
Two ring buffers are used, one as the sliding window for previous sections, and the other as the look-ahead buffer for incoming bytes. 256 binary search trees are built on top of the sliding window, one for every possible initial character. Each tree node corresponds to a series of bytes with different offsets but a constant length (maximum encoded length). Therefore there can be at most K nodes where K is the size of the sliding window. Nodes are inserted by lexicographical order. To search the longest match it first goes to the tree of the initial character, and traverse down the tree to find the longest match. When a new byte is pushed into the sliding window, all nodes that are affected by the new byte need to be erased and re-inserted into the tree. The compressor uses a greedy algorithm, meaning it will always find the longest match in each search.

### Decompressor
Decompressor maintains a single ring buffer as the sliding window for dictionary lookup, and simply go through each code till it reaches end of file.

## Complexity

Assume the file has N bytes, maximum offset is P, and maximum encoded length is Q.

### Compressor

* Runtime:
- Average: O(NQlog(P))
- Worst: O(NQP) when the binary search trees resemble linked lists
* Space: O(P+Q)

### Decompressor
* Runtime: O(N)
* Space: O(P)

## Tests

* nulltest: 0 byte
* smalltest: 38 bytes
* largetest: 6.5 MB
