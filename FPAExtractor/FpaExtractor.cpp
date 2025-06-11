#include "FpaExtractor.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <iomanip>
#include <direct.h>    // _mkdir
#include <errno.h>

FpaExtractor::FpaExtractor(const std::string& filePath)
    : filePath_(filePath)
{
    // Derive folder name from file name without extension
    auto pos = filePath_.find_last_of("/\\");
    std::string filename = (pos == std::string::npos ? filePath_ : filePath_.substr(pos + 1));
    auto dot = filename.find_last_of('.');
    outputFolder_ = (dot == std::string::npos ? filename : filename.substr(0, dot));
}

bool FpaExtractor::createOutputFolder() {
    int result = _mkdir(outputFolder_.c_str());
    return (result == 0 || errno == EEXIST);
}

void FpaExtractor::extract() {
    std::ifstream in(filePath_, std::ios::binary);
    if (!in) {
        std::cerr << "Failed to open input file: " << filePath_ << "\n";
        return;
    }

    if (!createOutputFolder()) {
        std::cerr << "Could not create output folder: " << outputFolder_ << "\n";
        return;
    }

    char window[4] = { 0 };
    size_t idx = 0;
    uint32_t chunkSize = 0;
    int fileCount = 1;

    // Read byte-by-byte
    char byte;
    while (in.get(byte)) {
        // Shift window
        window[idx % 4] = byte;
        idx++;

        // Check for "RIFF"
        if (idx >= 4) {
            std::string w(window + (idx % 4), window + 4);
            w += std::string(window, window + (idx % 4));
            if (w == RIFF_MAGIC) {
                // Read next 4 bytes as little-endian uint32_t
                if (!in.read(reinterpret_cast<char*>(&chunkSize), 4)) break;

                // Read chunkSize bytes of data
                std::vector<char> data(chunkSize);
                if (!in.read(data.data(), chunkSize)) break;

                // Construct output path
                std::ostringstream oss;
                oss << outputFolder_ << "\\" << outputFolder_
                    << "_" << std::setw(4) << std::setfill('0') << fileCount++ << ".WAV";
                std::ofstream out(oss.str(), std::ios::binary);
                if (out) {
                    // Write the header and data
                    out.write(RIFF_MAGIC, 4);
                    out.write(reinterpret_cast<char*>(&chunkSize), 4);
                    out.write(data.data(), chunkSize);
                }
                else {
                    std::cerr << "Failed to write output file: " << oss.str() << "\n";
                }
            }
        }
    }

    std::cout << "Extraction complete. " << (fileCount - 1) << " file(s) created." << std::endl;
}