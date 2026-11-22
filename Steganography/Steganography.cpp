#include "Steganography.h"
#include "Bits.h"
#include <cstdint>
#include <stdexcept>
#include <vector>

namespace
{
    // Liste les positions des octets de pixels utiles en sautant le padding de fin de ligne BMP.
    std::vector<std::size_t> GetUsableIndices(const BmpImage& img)
    {
        const std::size_t width = static_cast<std::size_t>(img.GetWidth());
        const std::size_t height = static_cast<std::size_t>(img.GetHeight());
        const std::size_t rowBytes = width * (static_cast<std::size_t>(img.GetBitsPerPixel()) / 8);
        const std::size_t stride = (rowBytes + 3) & ~static_cast<std::size_t>(3);

        if (img.GetPixels().size() < stride * height)
            throw std::runtime_error("Corrupted BMP pixel data");

        std::vector<std::size_t> indices;
        indices.reserve(rowBytes * height);
        for (std::size_t y = 0; y < height; ++y)
            for (std::size_t x = 0; x < rowBytes; ++x)
                indices.push_back(y * stride + x);

        return indices;
    }

    // Écrit les 8 bits d'un octet dans le bit de poids faible des 8 prochains octets de pixels.
    void WriteByteToLsb(std::vector<std::uint8_t>& pixels, const std::vector<std::size_t>& idx,
        std::size_t& bitCursor, std::uint8_t value)
    {
        for (int b = 7; b >= 0; --b)
        {
            const std::size_t p = idx[bitCursor++];
            pixels[p] = Bits::GetBit(value, static_cast<unsigned int>(b))
                ? Bits::SetBit(pixels[p], 0)
                : Bits::ClearBit(pixels[p], 0);
        }
    }

    // Reconstruit un octet en lisant le bit de poids faible des 8 prochains octets de pixels.
    std::uint8_t ReadByteFromLsb(const std::vector<std::uint8_t>& pixels, const std::vector<std::size_t>& idx,
        std::size_t& bitCursor)
    {
        std::uint8_t result = 0;
        for (int b = 7; b >= 0; --b)
            if (Bits::GetBit(pixels[idx[bitCursor++]], 0))
                result = Bits::SetBit(result, static_cast<unsigned int>(b));
        return result;
    }
}

// Calcule le nombre maximal de caractères que l'image peut dissimuler.
std::size_t Steganography::GetMaxCapacityBytes(const BmpImage& image)
{
    const std::size_t usableBytes = static_cast<std::size_t>(image.GetWidth()) *
        static_cast<std::size_t>(image.GetHeight()) *
        (static_cast<std::size_t>(image.GetBitsPerPixel()) / 8);
    return (usableBytes > 32) ? (usableBytes - 32) / 8 : 0;
}

// Chiffre ou déchiffre un texte en appliquant un XOR avec une clé répétée.
std::string Steganography::XorCipher(const std::string& data, const std::string& key)
{
    if (key.empty()) return data;

    std::string out = data;
    for (std::size_t i = 0; i < out.size(); ++i)
        out[i] = static_cast<char>(static_cast<unsigned char>(out[i]) ^
            static_cast<unsigned char>(key[i % key.size()]));
    return out;
}

// Cache la taille du message puis son contenu chiffré dans les pixels de l'image.
void Steganography::Embed(BmpImage& image, const std::string& message, const std::string& key)
{
    if (message.size() > GetMaxCapacityBytes(image))
        throw std::runtime_error("Message exceeds image capacity");

    const std::string payload = XorCipher(message, key);
    const std::uint32_t len = static_cast<std::uint32_t>(payload.size());
    const auto indices = GetUsableIndices(image);
    auto& pixels = image.GetPixels();
    std::size_t cursor = 0;

    for (int i = 0; i < 4; ++i)
        WriteByteToLsb(pixels, indices, cursor, static_cast<std::uint8_t>((len >> (i * 8)) & 0xFF));

    for (unsigned char c : payload)
        WriteByteToLsb(pixels, indices, cursor, c);
}

// Extrait la taille cachée puis lit et déchiffre le message contenu dans l'image.
std::string Steganography::Extract(const BmpImage& image, const std::string& key)
{
    const auto indices = GetUsableIndices(image);
    const auto& pixels = image.GetPixels();
    if (indices.size() < 32)
        throw std::runtime_error("Image too small");

    std::size_t cursor = 0;
    std::uint32_t len = 0;

    for (int i = 0; i < 4; ++i)
        len |= static_cast<std::uint32_t>(ReadByteFromLsb(pixels, indices, cursor)) << (i * 8);

    if (len > GetMaxCapacityBytes(image))
        throw std::runtime_error("Invalid or corrupted hidden message");

    std::string encrypted(len, '\0');
    for (std::uint32_t i = 0; i < len; ++i)
        encrypted[i] = static_cast<char>(ReadByteFromLsb(pixels, indices, cursor));

    return XorCipher(encrypted, key);
}