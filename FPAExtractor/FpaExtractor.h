#pragma once

#include <string>
#include <cstdint>

class FpaExtractor {
public:
    explicit FpaExtractor(const std::string& filePath);
    void extract();

private:
    std::string filePath_;       // Path to the .FPA file
    std::string outputFolder_;   // Derived folder name
    static constexpr char RIFF_MAGIC[5] = "RIFF";

    bool createOutputFolder();
};