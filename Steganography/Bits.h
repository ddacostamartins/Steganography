#pragma once

#include <cstdint>
#include <iostream>

class Bits
{
public:
    static bool GetBit(std::uint8_t value, unsigned int index);
    static std::uint8_t SetBit(std::uint8_t value, unsigned int index);
    static std::uint8_t ClearBit(std::uint8_t value, unsigned int index);
    static void PrintBits(std::uint8_t value);
    static std::uint8_t ToggleBit(std::uint8_t value, unsigned int index);
};