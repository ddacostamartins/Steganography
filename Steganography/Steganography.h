#pragma once

#include "Files.h"
#include <cstddef>
#include <string>

class Steganography
{
public:
    static std::size_t GetMaxCapacityBytes(const BmpImage& image);
    static std::string XorCipher(const std::string& data, const std::string& key);
    static void Embed(BmpImage& image, const std::string& message, const std::string& key = "");
    static std::string Extract(const BmpImage& image, const std::string& key = "");
};