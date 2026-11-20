#include "BmpImage.hpp"

int main()
{
    BmpImage img("original.bmp");

    // Récupère une référence modifiable vers tous les octets RGB(A)
    auto& pixels = img.GetPixels();

    // Exemple : modifier le bit de poids faible (LSB) du premier octet de couleur
    pixels[0] = (pixels[0] & 0xFE) | 1;

    // Sauvegarde le résultat dans un nouveau fichier BMP valide
    img.Save("image_secrete.bmp");
}