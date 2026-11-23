#ifndef KASIR_H
#define KASIR_H

#include <string>
#include <vector>

struct Barang {
    std::string nama;
    double harga;
    int jumlah;
};

void tampilkanMenu();
void tambahBarang(std::vector<Barang>& keranjang);
void tampilkanKeranjang(const std::vector<Barang>& keranjang);
void editJumlahBarang(std::vector<Barang>& keranjang);
void hapusBarang(std::vector<Barang>& keranjang);
void prosesPembayaran(std::vector<Barang>& keranjang);
void simpanStrukFile(
    const std::vector<Barang>& keranjang,
    double subtotal,
    double diskon,
    double totalAkhir,
    double uangBayar,
    double kembalian

);

#endif