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

    while ((c = getopt(argc, argv, "di:o:")) != -1) {
        switch (c) {
            case 'd':   // decompress mode
                dflag = true;
                break;
            case 'i':   // input file name
                ifname = optarg;
                break;
            case 'o':   // output file name
                ofname = optarg;
                break;
            case '?':   // error
                if (optopt == 'i' || optopt == 'o') {
                    cerr << "Option -" << static_cast<char>(optopt) << " requires an argument.\n" << endl;
                } else {
                    cerr << "Unknown option '-" << static_cast<char>(optopt) << "'.\n" << endl;
                }
            default:
                return 1;
        }
    }

    // check required fields
    if (ifname.empty() || ofname.empty()) {
        cerr << "Usage: ezip [-d] -i <input file name> -o <output file name>" << endl;
    }

    if (dflag) {
        Decompressor(ifname, ofname).decompress();
    } else {
        Compressor(ifname, ofname).compress();
    }
}
