#ifndef KASIR_H
#define KASIR_H

#include <string>
#include <vector>

struct Barang {
    std::string nama;
    double harga;
    int jumlah;
};

// Deklarasi fungsi-fungsi utama
void tampilkanMenu();
void tambahBarang(std::vector<Barang>& keranjang);
void tampilkanKeranjang(const std::vector<Barang>& keranjang);
void prosesPembayaran(std::vector<Barang>& keranjang);

#endif