#ifndef MESIN_KASIR_H
#define MESIN_KASIR_H

#include "Inventaris.h"
#include <vector>
#include <string>

class MesinKasir {
private:
    Inventaris& inventaris;
    std::vector<ItemKeranjang> keranjang;
    std::string pathRiwayat;
    const std::string PIN_ADMIN = "1234";

    void menuModeKasir();
    void menuModeAdmin();

    void tambahKeKeranjang();
    void tampilkanKeranjang() const;
    void editJumlahKeranjang();
    void hapusDariKeranjang();
    void prosesPembayaran();

    void cetakStrukKeFile(double subtotal, double diskon, double totalAkhir, double tunai, double kembalian) const;
    void catatRiwayatTransaksi(int totalItem, double totalAkhir) const;
    void tampilkanLaporanPenjualan() const;

public:
    MesinKasir(Inventaris& inv, const std::string& fileRiwayat);
    void jalankan();
};

#endif