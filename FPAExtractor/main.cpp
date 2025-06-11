#include "FpaExtractor.h"
#include <iostream>

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: FpaExtractor <path_to_file.FPA>\n";
        return 1;
    }

    FpaExtractor extractor(argv[1]);
    extractor.extract();

    return 0;
}
