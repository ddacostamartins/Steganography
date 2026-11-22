#include "Bits.h"
#include <iostream>

// Récupère la valeur (0 ou 1) du bit situé à la position index.
bool Bits::GetBit(std::uint8_t value, unsigned int index)
{
    return (value >> index) & 1u;
}

// Force à 1 le bit situé à la position index.
std::uint8_t Bits::SetBit(std::uint8_t value, unsigned int index)
{
    return static_cast<std::uint8_t>(value | (1u << index));
}

// Force à 0 le bit situé à la position index.
std::uint8_t Bits::ClearBit(std::uint8_t value, unsigned int index)
{
    return static_cast<std::uint8_t>(value & ~(1u << index));
}

// Inverse l'état (0 devient 1, 1 devient 0) du bit situé à la position index.
std::uint8_t Bits::ToggleBit(std::uint8_t value, unsigned int index)
{
    return static_cast<std::uint8_t>(value ^ (1u << index));
}

// Affiche la représentation binaire sur 8 bits d'un octet dans la console.
void Bits::PrintBits(std::uint8_t value)
{
    for (int i = 7; i >= 0; --i)
    {
        std::cout << (GetBit(value, static_cast<unsigned int>(i)) ? '1' : '0');
    }
}