#include "compressor.h"
#include "decompressor.h"
#include <cctype>
#include <iostream>
#include <string>
#include <unistd.h>

using std::cerr; using std::endl;
using std::string;

int main(int argc, char *argv[])
{
    int c;
    bool dflag = false;
    string ifname;
    string ofname;

    opterr = 0;

    while ((c = getopt(argc, argv, "di:o:")) != -1) {
        switch (c) {
            case 'd':
                dflag = true;
                break;
            case 'i':
                ifname = optarg;
                break;
            case 'o':
                ofname = optarg;
                break;
            case '?':
                if (optopt == 'i' || optopt == 'o') {
                    cerr << "Option -" << static_cast<char>(optopt) << " requires an argument.\n" << endl;
                } else {
                    cerr << "Unknown option '-" << static_cast<char>(optopt) << "'.\n" << endl;
                }
                return 1;
            default:
                abort();
        }
    }

    if (ifname.empty() || ofname.empty()) {
        cerr << "Usage: ezip [-d] -i <input file name> -o <output file name>" << endl;
    }

    if (dflag) {
        Decompressor(ifname, ofname).decompress();
    } else {
        Compressor(ifname, ofname).compress();
    }
}
