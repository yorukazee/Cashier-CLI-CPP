#ifndef UI_HELPER_H
#define UI_HELPER_H

#include <iostream>
#include <string>
#include <limits>

namespace UI {
    // Kode Warna ANSI
    inline const std::string RESET   = "\033[0m";
    inline const std::string BOLD    = "\033[1m";
    inline const std::string MERAH   = "\033[31m";
    inline const std::string HIJAU   = "\033[32m";
    inline const std::string KUNING  = "\033[33m";
    inline const std::string BIRU    = "\033[34m";
    inline const std::string CYAN    = "\033[36m";

    inline void cetakSukses(const std::string& pesan) {
        std::cout << HIJAU << "-> " << pesan << RESET << "\n";
    }

    inline void cetakError(const std::string& pesan) {
        std::cout << MERAH << "[!] " << pesan << RESET << "\n";
    }

    inline void cetakPeringatan(const std::string& pesan) {
        std::cout << KUNING << "[!] " << pesan << RESET << "\n";
    }

    inline void bersihkanBuffer() {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }

    inline int bacaInt(const std::string& prompt, int minVal = std::numeric_limits<int>::min(), int maxVal = std::numeric_limits<int>::max()) {
        int nilai;
        while (true) {
            std::cout << prompt;
            if (std::cin >> nilai && nilai >= minVal && nilai <= maxVal) {
                bersihkanBuffer();
                return nilai;
            }
            cetakError("Input angka tidak valid! Coba lagi.");
            bersihkanBuffer();
        }
    }

    inline double bacaDouble(const std::string& prompt, double minVal = 0.0) {
        double nilai;
        while (true) {
            std::cout << prompt;
            if (std::cin >> nilai && nilai >= minVal) {
                bersihkanBuffer();
                return nilai;
            }
            cetakError("Input nominal tidak valid! Coba lagi.");
            bersihkanBuffer();
        }
    }

    inline std::string bacaBaris(const std::string& prompt) {
        std::string teks;
        std::cout << prompt;
        std::getline(std::cin, teks);
        return teks;
    }
}

#endif