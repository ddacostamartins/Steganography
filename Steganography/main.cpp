#include "Files.h"
#include <iostream>

int main()
{
    try
    {
        BmpImage img("C:/Users/David/source/repos/Steganography/8x8.bmp");

        std::cout << "=== Image chargee avec succes ===" << '\n';
        std::cout << "Dimensions : " << img.GetWidth() << "x" << img.GetHeight() << '\n';
        std::cout << "Profondeur : " << img.GetBitsPerPixel() << " bits" << '\n';
        std::cout << "Taille des donnees pixels : " << img.GetPixels().size() << " octets\n\n";

        auto& pixels = img.GetPixels();
        std::cout << "Premier octet avant : " << static_cast<int>(pixels[0]) << '\n';

        pixels[0] = (pixels[0] & 0xFE) | 1;
        std::cout << "Premier octet apres : " << static_cast<int>(pixels[0]) << "\n\n";

        img.Save("8x8_modifie.bmp");
        std::cout << "Sauvegarde dans 8x8_modifie.bmp reussie !" << '\n';

        BmpImage imgVerif("8x8_modifie.bmp");
        std::cout << "Verification : premier octet lu = "
            << static_cast<int>(imgVerif.GetPixels()[0]) << '\n';
    }
    catch (const std::exception& e)
    {
        std::cerr << "Erreur : " << e.what() << '\n';
        return 1;
    }

    return 0;
}