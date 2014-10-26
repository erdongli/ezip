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
Two ring buffers are used, one as the sliding window for previous sections, and the other as the look-ahead buffer for incoming bytes. 256 binary search trees are built on top of the sliding window, one for every possible initial character. Each tree node corresponds to a series of bytes with different offsets but a constant length (maximum encoded length). Therefore there can be at most K nodes where K is the size of the sliding window. Nodes are inserted in lexicographical order. To find the longest match it first goes to the root with the same initial character, and traverse down the tree.

When a new byte is pushed into the sliding window, all nodes that are affected need to be erased and re-inserted into the corresponding trees. The compressor uses a greedy algorithm, meaning it will always find the longest match in each search for better runtime performance.

### Decompressor
Decompressor maintains a single ring buffer as the sliding window for dictionary lookup. It simply goes through each code till end of file.

## Complexity

Assume the input file has N bytes, maximum offset is P, and maximum encoded length is Q.

### Compressor

* Runtime:
  * Average: O(N*Q*log(P)). It takes O(N*log(P)) for the searches, and O(N*Q*log(P)) for the updates.
  * Worst: O(NQP) when the binary search trees resemble linked lists
* Space: O(P+Q)

### Decompressor
* Runtime: O(N)
* Space: O(P)

## Tests

* nulltest: 0 byte
* smalltest: 38 bytes
* largetest: 6.5 MB
