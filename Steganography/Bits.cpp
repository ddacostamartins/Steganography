#include "Bits.h"

bool Bits::GetBit(std::uint8_t value, unsigned int index)
{
    return (value >> index) & 1u;
}

std::uint8_t Bits::SetBit(std::uint8_t value, unsigned int index)

{
    return value | (1u << index);
}

std::uint8_t Bits::ClearBit(std::uint8_t value, unsigned int index)
{
    return value & ~(1u << index);
}

void Bits::PrintBits(std::uint8_t value)
{
    for (int i = 7; i >= 0; --i)
    {
        std::cout << (GetBit(value, i) ? "1" : "0");
    }
}

std::uint8_t Bits::ToggleBit(std::uint8_t value, unsigned int index)
{
    return static_cast<std::uint8_t>(value ^ (1u << index));
}

