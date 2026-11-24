#ifndef MESIN_KASIR_H
#define MESIN_KASIR_H

#include "Database.h"
#include "AuthManager.h"
#include "Inventaris.h"
#include <vector>
#include <string>

class MesinKasir {
private:
    Database& db;
    AuthManager& auth;
    Inventaris& inventaris;
    std::vector<ItemKeranjang> keranjang;

    void menuKasir();
    void menuAdmin();
    void menuKelolaPromoDanPajak();
    void menuKelolaUser();
    void menuLaporanAnalitik() const;

    // Operasi Transaksi
    void tambahKeKeranjang();
    void tampilkanKeranjang() const;
    void editJumlahKeranjang();
    void hapusDariKeranjang();
    void prosesPembayaran();

    // Helper Promo, Pajak, Database Transaksi, & Struk
    double cekPersenPromo(const std::string& kodePromo) const;
    bool isPpnAktif() const;
    double getTarifPpn() const;
    int simpanTransaksiKeDB(double subtotal, double diskon, double ppn, double totalAkhir) const;
    void cetakStrukKeFile(int idTransaksi, const std::string& kodePromo, double subtotal, double diskon, double ppn, double totalAkhir, double tunai, double kembalian) const;

    // Laporan Analitik
    void laporanSemuaTransaksi() const;
    void laporanBerdasarkanTanggal(const std::string& tanggal) const;
    void laporanBerdasarkanKasir(const std::string& usernameKasir) const;
    void laporanProdukTerlaris() const;

public:
    MesinKasir(Database& database, AuthManager& authManager, Inventaris& inv);
    void jalankan();
};

#endif