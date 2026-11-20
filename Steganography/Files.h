#pragma once

#include <array>
#include <cstdint>
#include <string>
#include <vector>

class BmpImage
{
public:
    explicit BmpImage(const std::string& filename);

    // Sauvegarde l'image (en-tête + pixels modifiés) dans un nouveau fichier
    void Save(const std::string& filename) const;

    // Accès aux pixels pour cacher / lire le message
    std::vector<std::uint8_t>& GetPixels();
    const std::vector<std::uint8_t>& GetPixels() const;

    std::int32_t GetWidth() const;
    std::int32_t GetHeight() const;
    std::uint16_t GetBitsPerPixel() const;

private:
    std::array<char, 54> m_header{};
    std::vector<std::uint8_t> m_pixels;

    void Load(const std::string& filename);
    void CheckSignature() const;
    void CheckConstraints() const;

    std::uint16_t ReadUint16(std::size_t offset) const;
    std::uint32_t ReadUint32(std::size_t offset) const;
};