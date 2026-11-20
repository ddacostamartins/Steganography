#include "BmpImage.hpp"
#include <fstream>
#include <stdexcept>

BmpImage::BmpImage(const std::string& filename)
{
    Load(filename);
}

void BmpImage::Load(const std::string& filename)
{
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open())
        throw std::runtime_error("Cannot open file");

    // 1. Lecture et validation de l'en-tête (54 octets)
    file.read(m_header.data(), m_header.size());
    if (file.gcount() != m_header.size())
        throw std::runtime_error("File is too small to contain a valid BMP header");

    CheckSignature();
    CheckConstraints();

    // 2. Positionnement au début réel des pixels (offset stocké à l'octet 10)
    const std::uint32_t pixelOffset = ReadUint32(10);
    file.seekg(0, std::ios::end);
    const std::streamsize fileSize = file.tellg();

    if (fileSize <= static_cast<std::streamsize>(pixelOffset))
        throw std::runtime_error("BMP file contains no pixel data");

    // 3. Chargement de tous les octets de pixels en mémoire
    const std::size_t pixelDataSize = static_cast<std::size_t>(fileSize) - pixelOffset;
    m_pixels.resize(pixelDataSize);

    file.seekg(pixelOffset, std::ios::beg);
    file.read(reinterpret_cast<char*>(m_pixels.data()), pixelDataSize);
}

void BmpImage::Save(const std::string& filename) const
{
    std::ofstream file(filename, std::ios::binary);
    if (!file.is_open())
        throw std::runtime_error("Cannot create output file");

    // Met à jour l'offset des pixels à 54 au cas où l'original avait un espace vide après le header
    std::array<char, 54> outHeader = m_header;
    outHeader[10] = 54;
    outHeader[11] = 0;
    outHeader[12] = 0;
    outHeader[13] = 0;

    file.write(outHeader.data(), outHeader.size());
    file.write(reinterpret_cast<const char*>(m_pixels.data()), m_pixels.size());
}

std::vector<std::uint8_t>& BmpImage::GetPixels()
{
    return m_pixels;
}

const std::vector<std::uint8_t>& BmpImage::GetPixels() const
{
    return m_pixels;
}

std::int32_t BmpImage::GetWidth() const
{
    return static_cast<std::int32_t>(ReadUint32(18));
}

std::int32_t BmpImage::GetHeight() const
{
    return static_cast<std::int32_t>(ReadUint32(22));
}

std::uint16_t BmpImage::GetBitsPerPixel() const
{
    return ReadUint16(28);
}

void BmpImage::CheckSignature() const
{
    if (static_cast<unsigned char>(m_header[0]) != 0x42 ||
        static_cast<unsigned char>(m_header[1]) != 0x4D)
    {
        throw std::runtime_error("File is not a valid BMP file (missing BM signature)");
    }
}

void BmpImage::CheckConstraints() const
{
    const std::uint32_t dibHeaderSize = ReadUint32(14);
    const std::uint16_t planes = ReadUint16(26);
    const std::uint16_t bitsPerPixel = GetBitsPerPixel();
    const std::uint32_t compression = ReadUint32(30);
    const std::int32_t width = GetWidth();
    const std::int32_t height = GetHeight();

    if (dibHeaderSize != 40)
        throw std::runtime_error("Invalid DIB header size. Expected 40 bytes, got " + std::to_string(dibHeaderSize));

    if (planes != 1)
        throw std::runtime_error("Invalid number of planes. Expected 1, got " + std::to_string(planes));

    if (bitsPerPixel != 24 && bitsPerPixel != 32)
        throw std::runtime_error("Invalid bits per pixel. Expected 24 or 32, got " + std::to_string(bitsPerPixel));

    if (compression != 0)
        throw std::runtime_error("Invalid compression method. Expected 0, got " + std::to_string(compression));

    if (width <= 0 || height <= 0)
        throw std::runtime_error("Invalid image dimensions. Got " + std::to_string(width) + "x" + std::to_string(height));
}

std::uint16_t BmpImage::ReadUint16(std::size_t offset) const
{
    return static_cast<std::uint16_t>(
        static_cast<unsigned char>(m_header[offset]) |
        (static_cast<unsigned char>(m_header[offset + 1]) << 8));
}

std::uint32_t BmpImage::ReadUint32(std::size_t offset) const
{
    return static_cast<std::uint32_t>(
        static_cast<unsigned char>(m_header[offset]) |
        (static_cast<std::uint32_t>(static_cast<unsigned char>(m_header[offset + 1])) << 8) |
        (static_cast<std::uint32_t>(static_cast<unsigned char>(m_header[offset + 2])) << 16) |
        (static_cast<std::uint32_t>(static_cast<unsigned char>(m_header[offset + 3])) << 24));
}