#include  "Files.h"

// Reading from a binary file
void ReadBinaryFile(const std::string& filename)
{
    std::ifstream file(filename, std::ios::binary);

    if (!file.is_open())
    {
        throw std::runtime_error("Cannot open file");
    }

    constexpr std::size_t buffer_size = 500;
    char buffer[buffer_size];

    file.read(buffer, buffer_size);

    std::streamsize bytes_read = file.gcount();

    std::string result(
        buffer,
        static_cast<std::size_t>(bytes_read)
    );
}

// Writing to a binary file
void WriteBinaryFile(const std::string& filename, const std::string& text)
{
    std::ofstream file(filename, std::ios::binary);

    if (!file.is_open())
    {
        throw std::runtime_error("Cannot open file");
    }

    file.write(text.data(), text.size());
    file.close();
}

// Vérifie si le fichier est un fichier BMP valide
bool IsValidBmpFile(const std::string& filename)
{
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open())
    {
        throw std::runtime_error("Cannot open file");
    }

    char header[54] = {};
    file.read(header, sizeof(header));
    if (file.gcount() != sizeof(header))
    {
        throw std::runtime_error("Files is too small to be a valid BMP file");
        return false;
    }

    if (static_cast<unsigned char>(header[0]) != 0x42 ||
        static_cast<unsigned char>(header[1]) != 0x4D)
    {
        throw std::runtime_error("File is not a BMP file");
        return false;
    }

    const auto readUint16 = [&header](std::size_t offset)
    {
        return static_cast<std::uint16_t>(
            static_cast<unsigned char>(header[offset]) |
            (static_cast<unsigned char>(header[offset + 1]) << 8));
    };

    const auto readUint32 = [&header](std::size_t offset)
    {
        return static_cast<std::uint32_t>(
            static_cast<unsigned char>(header[offset]) |
            (static_cast<std::uint32_t>(static_cast<unsigned char>(header[offset + 1])) << 8) |
            (static_cast<std::uint32_t>(static_cast<unsigned char>(header[offset + 2])) << 16) |
            (static_cast<std::uint32_t>(static_cast<unsigned char>(header[offset + 3])) << 24));
    };

    const std::uint32_t dibHeaderSize = readUint32(14);
    const std::uint16_t planes = readUint16(26);
    const std::uint16_t bitsPerPixel = readUint16(28);
    const std::uint32_t compression = readUint32(30);

    const std::uint32_t widthBits = readUint32(18);
    const std::uint32_t heightBits = readUint32(22);
    const std::int32_t width = static_cast<std::int32_t>(widthBits);
    const std::int32_t height = static_cast<std::int32_t>(heightBits);

    if (dibHeaderSize == 40 &&
        planes == 1 &&
        (bitsPerPixel == 24 || bitsPerPixel == 32) &&
        compression == 0 &&
        width > 0 && height > 0)
    {
        return true;
    }

    if (!(dibHeaderSize == 40))
        throw std::runtime_error("Invalid DIB header size. Expected 40 bytes, got " + std::to_string(dibHeaderSize));

    if (!(planes == 1))
        throw std::runtime_error("Invalid number of planes. Expected 1, got " + std::to_string(planes));
    
    if (!(bitsPerPixel == 24 || bitsPerPixel == 32))
        throw std::runtime_error("Invalid bits per pixel. Expected 24 or 32, got " + std::to_string(bitsPerPixel));
    
    if (!(compression == 0))
        throw std::runtime_error("Invalid compression method. Expected 0 (no compression), got " + std::to_string(compression));

    if (!(width > 0 && height > 0))
        throw std::runtime_error("Invalid image dimensions. Width and height must be positive. Got width: " + std::to_string(width) + ", height: " + std::to_string(height));
    
    return false;
}