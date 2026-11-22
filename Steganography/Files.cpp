#include "Files.h"
#include <fstream>
#include <stdexcept>

// Construit l'objet et charge immédiatement le fichier BMP spécifié.
BmpImage::BmpImage(const std::string& filename)
{
    Load(filename);
}

// Ouvre le fichier BMP, valide son en-tête et charge les octets des pixels en mémoire.
void BmpImage::Load(const std::string& filename)
{
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open())
        throw std::runtime_error("Cannot open file");

    file.read(m_header.data(), m_header.size());
    if (file.gcount() != m_header.size())
        throw std::runtime_error("File is too small to contain a valid BMP header");

    CheckSignature();
    CheckConstraints();

    const std::uint32_t pixelOffset = ReadUint32(10);
    file.seekg(0, std::ios::end);
    const std::streamsize fileSize = file.tellg();

    if (fileSize <= static_cast<std::streamsize>(pixelOffset))
        throw std::runtime_error("BMP file contains no pixel data");

    const std::size_t pixelDataSize = static_cast<std::size_t>(fileSize) - pixelOffset;
    m_pixels.resize(pixelDataSize);

    file.seekg(pixelOffset, std::ios::beg);
    file.read(reinterpret_cast<char*>(m_pixels.data()), pixelDataSize);
}

// Écrit l'en-tête BMP et les données pixels modifiées dans un nouveau fichier binaire.
void BmpImage::Save(const std::string& filename) const
{
    std::ofstream file(filename, std::ios::binary);
    if (!file.is_open())
        throw std::runtime_error("Cannot create output file");

    std::array<char, 54> outHeader = m_header;
    outHeader[10] = 54;
    outHeader[11] = 0;
    outHeader[12] = 0;
    outHeader[13] = 0;

    file.write(outHeader.data(), outHeader.size());
    file.write(reinterpret_cast<const char*>(m_pixels.data()), m_pixels.size());
}

// Retourne une référence modifiable vers le tableau des octets de pixels.
std::vector<std::uint8_t>& BmpImage::GetPixels()
{
    return m_pixels;
}

// Retourne une référence en lecture seule vers le tableau des octets de pixels.
const std::vector<std::uint8_t>& BmpImage::GetPixels() const
{
    return m_pixels;
}

// Lit et retourne la largeur de l'image en pixels depuis l'en-tête BMP.
std::int32_t BmpImage::GetWidth() const
{
    return static_cast<std::int32_t>(ReadUint32(18));
}

// Lit et retourne la hauteur de l'image en pixels depuis l'en-tête BMP.
std::int32_t BmpImage::GetHeight() const
{
    return static_cast<std::int32_t>(ReadUint32(22));
}

// Lit et retourne la profondeur de couleur (nombre de bits par pixel) de l'image.
std::uint16_t BmpImage::GetBitsPerPixel() const
{
    return ReadUint16(28);
}

// Vérifie que les deux premiers octets du fichier correspondent bien à la signature "BM".
void BmpImage::CheckSignature() const
{
    if (static_cast<unsigned char>(m_header[0]) != 0x42 ||
        static_cast<unsigned char>(m_header[1]) != 0x4D)
    {
        throw std::runtime_error("File is not a valid BMP file (missing BM signature)");
    }
}

// Vérifie que le format BMP respecte les contraintes requises (DIB valide, 24/32 bits, non compressé).
void BmpImage::CheckConstraints() const
{
    const std::uint32_t dibHeaderSize = ReadUint32(14);
    const std::uint16_t planes = ReadUint16(26);
    const std::uint16_t bitsPerPixel = GetBitsPerPixel();
    const std::uint32_t compression = ReadUint32(30);
    const std::int32_t width = GetWidth();
    const std::int32_t height = GetHeight();

    if (dibHeaderSize != 40 && dibHeaderSize != 108 && dibHeaderSize != 124)
        throw std::runtime_error("Unsupported DIB header size: " + std::to_string(dibHeaderSize));

    if (planes != 1)
        throw std::runtime_error("Invalid number of planes. Expected 1, got " + std::to_string(planes));

    if (bitsPerPixel != 24 && bitsPerPixel != 32)
        throw std::runtime_error("Invalid bits per pixel. Expected 24 or 32, got " + std::to_string(bitsPerPixel));

    if (compression != 0 && compression != 3)
        throw std::runtime_error("Compressed BMP files are not supported");

    if (width <= 0 || height <= 0)
        throw std::runtime_error("Invalid image dimensions");
}

// Lit un entier non signé sur 16 bits (little-endian) à la position donnée dans l'en-tête.
std::uint16_t BmpImage::ReadUint16(std::size_t offset) const
{
    return static_cast<std::uint16_t>(
        static_cast<unsigned char>(m_header[offset]) |
        (static_cast<unsigned char>(m_header[offset + 1]) << 8));
}

// Lit un entier non signé sur 32 bits (little-endian) à la position donnée dans l'en-tête.
std::uint32_t BmpImage::ReadUint32(std::size_t offset) const
{
    return static_cast<std::uint32_t>(
        static_cast<unsigned char>(m_header[offset]) |
        (static_cast<std::uint32_t>(static_cast<unsigned char>(m_header[offset + 1])) << 8) |
        (static_cast<std::uint32_t>(static_cast<unsigned char>(m_header[offset + 2])) << 16) |
        (static_cast<std::uint32_t>(static_cast<unsigned char>(m_header[offset + 3])) << 24));
}