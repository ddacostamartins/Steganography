#include "Files.h"
#include "Steganography.h"
#include <iostream>
#include <stdexcept>
#include <string>

namespace
{
    // Vérifie que le nombre d'arguments passés en ligne de commande est valide.
    void CheckArgs(int argc, int minArgs, int maxArgs)
    {
        if (argc < minArgs || argc > maxArgs)
            throw std::invalid_argument("Invalid number of arguments (use --help)");
    }

    // Charge l'image source, y dissimule le message et sauvegarde le résultat.
    void CmdEncode(int argc, char* argv[])
    {
        CheckArgs(argc, 5, 6);
        BmpImage img(argv[2]);
        Steganography::Embed(img, argv[4], argc == 6 ? argv[5] : "");
        img.Save(argv[3]);
        std::cout << "Encoded into '" << argv[3] << "'.\n";
    }

    // Lit et affiche le message caché dans l'image fournie.
    void CmdDecode(int argc, char* argv[])
    {
        CheckArgs(argc, 3, 4);
        const BmpImage img(argv[2]);
        std::cout << Steganography::Extract(img, argc == 4 ? argv[3] : "") << '\n';
    }

    // Affiche les dimensions, la profondeur de couleur et la capacité maximale de l'image.
    void CmdInfo(int argc, char* argv[])
    {
        CheckArgs(argc, 3, 3);
        const BmpImage img(argv[2]);
        std::cout << img.GetWidth() << "x" << img.GetHeight() << " ("
            << img.GetBitsPerPixel() << " bpp) - Capacity: "
            << Steganography::GetMaxCapacityBytes(img) << " bytes\n";
    }

    // Teste automatiquement l'encodage, la sauvegarde, la relecture et le décodage d'un message.
    void CmdVerify(int argc, char* argv[])
    {
        CheckArgs(argc, 4, 5);
        const std::string msg = argv[3];
        const std::string key = (argc == 5) ? argv[4] : "";

        BmpImage img(argv[2]);
        Steganography::Embed(img, msg, key);
        img.Save("verify_tmp.bmp");

        const BmpImage reloaded("verify_tmp.bmp");
        if (Steganography::Extract(reloaded, key) != msg)
            throw std::runtime_error("[FAIL] Extracted message differs from original");

        std::cout << "[PASS] Round-trip verification succeeded.\n";
    }

    // Retire les guillemets autour d'un chemin si l'utilisateur a glissé-déposé le fichier.
    std::string CleanPath(std::string path)
    {
        if (path.size() >= 2 && path.front() == '"' && path.back() == '"')
            return path.substr(1, path.size() - 2);
        return path;
    }

    // Lance un menu interactif pour choisir n'importe quelle image sans toucher au code.
    void RunInteractiveMenu()
    {
        std::cout << "=== Steganographie BMP ===\n"
            << "1. Cacher un message (Encode)\n"
            << "2. Lire un message (Decode)\n"
            << "3. Infos sur une image (Info)\n"
            << "Choix : ";

        std::string choice;
        std::getline(std::cin, choice);

        if (choice == "1")
        {
            std::string input, output, message, key;
            std::cout << "Chemin de l'image BMP d'origine (glisser-deposer possible) : ";
            std::getline(std::cin, input);
            std::cout << "Nom de l'image BMP de sortie (ex: sortie.bmp) : ";
            std::getline(std::cin, output);
            std::cout << "Message secret : ";
            std::getline(std::cin, message);
            std::cout << "Cle de chiffrement (laisser vide si aucune) : ";
            std::getline(std::cin, key);

            BmpImage img(CleanPath(input));
            Steganography::Embed(img, message, key);
            img.Save(CleanPath(output));
            std::cout << "Succes ! Message cache dans '" << CleanPath(output) << "'.\n";
        }
        else if (choice == "2")
        {
            std::string input, key;
            std::cout << "Chemin de l'image BMP a analyser : ";
            std::getline(std::cin, input);
            std::cout << "Cle de chiffrement (laisser vide si aucune) : ";
            std::getline(std::cin, key);

            const BmpImage img(CleanPath(input));
            std::cout << "Message trouve : " << Steganography::Extract(img, key) << '\n';
        }
        else if (choice == "3")
        {
            std::string input;
            std::cout << "Chemin de l'image BMP : ";
            std::getline(std::cin, input);

            const BmpImage img(CleanPath(input));
            std::cout << "Dimensions : " << img.GetWidth() << "x" << img.GetHeight()
                << " (" << img.GetBitsPerPixel() << " bpp)\n"
                << "Capacite maximale : " << Steganography::GetMaxCapacityBytes(img) << " octets\n";
        }
        else
        {
            throw std::invalid_argument("Choix invalide (tapez 1, 2 ou 3).");
        }
    }
}

// Point d'entrée du programme : gère les arguments CLI ou ouvre le menu interactif.
int main(int argc, char* argv[])
{
    try
    {
        if (argc == 1)
        {
            RunInteractiveMenu();
            return 0;
        }

        const std::string cmd = argv[1];

        if (cmd == "--help" || cmd == "-h")
            std::cout << "Usage: stego <encode|decode|info|verify> <args...>\n";
        else if (cmd == "encode") CmdEncode(argc, argv);
        else if (cmd == "decode") CmdDecode(argc, argv);
        else if (cmd == "info")   CmdInfo(argc, argv);
        else if (cmd == "verify") CmdVerify(argc, argv);
        else throw std::invalid_argument("Unknown command: " + cmd);
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << '\n';
        return 1;
    }
    return 0;
}