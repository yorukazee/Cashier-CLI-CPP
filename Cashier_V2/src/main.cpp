#include "../include/kasir.h"
#include <iostream>
#include <limits>

int main() {
    std::vector<Barang> keranjang;
    int pilihan = -1;

    do{
        tampilkanMenu();

        if (!(std::cin >> pilihan)) {
            std:: cout << "Harap masukkan angka yang valid!\n";
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
                editJumlahBarang(keranjang);
                break;
            case 4:
                hapusBarang(keranjang);
                break;
            case 5:
                prosesPembayaran(keranjang);
                break;
            case 0:
                std::cout << "Terima kasih telah menggunakan aplikasi kasir!\n";
                break;
            default:
                std::cout << "Pilihan tidak valid! Silakan pilih menu yang tersedia.\n";
        }
    } while (pilihan != 0);

     return 0;

}