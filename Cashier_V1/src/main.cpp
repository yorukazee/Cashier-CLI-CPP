#include "../include/kasir.h"
#include <iostream>
#include <limits>

int main() {
    std::vector<Barang> keranjang;
    int pilihan = -1;

    do {
        tampilkanMenu();
        
        // Cegah infinite loop jika user mengetik huruf di menu utama
        if (!(std::cin >> pilihan)) {
            std::cout << "Harap masukkan angka menu yang valid!\n";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            continue;
        }

        switch (pilihan) {
            case 1:
                tambahBarang(keranjang);
                break;
            case 2:
                tampilkanKeranjang(keranjang);
                break;
            case 3:
                prosesPembayaran(keranjang);
                break;
            case 0:
                std::cout << "Terima kasih telah menggunakan aplikasi kasir!\n";
                break;
            default:
                std::cout << "Pilihan tidak tersedia.\n";
        }
    } while (pilihan != 0);

    return 0;
}