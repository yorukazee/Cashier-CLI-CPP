#ifndef INVENTARIS_H
#define INVENTARIS_H

#include "Database.h"
#include <string>
#include <vector>
#include <optional>

struct Produk {
    std::string kode;
    std::string nama;
    double harga;
    int stok;
};

struct ItemKeranjang {
    std::string kode;
    std::string nama;
    double harga;
    int jumlah;
};

class Inventaris {
private:
    Database& db;

public:
    explicit Inventaris(Database& database);

    void tampilkanKatalog() const;
    void cekStokMenipis(int batasMinimum = 5) const;
    void cariProduk(const std::string& kataKunci) const;

    std::optional<Produk> cariBerdasarkanKode(const std::string& kode) const;
    bool tambahProdukBaru(const Produk& produkBaru);
    bool ubahHargaProduk(const std::string& kode, double hargaBaru);
    bool tambahStokProduk(const std::string& kode, int tambahanStok);
    bool hapusProduk(const std::string& kode);
    bool kurangiStokSetelahTransaksi(const std::vector<ItemKeranjang>& keranjang);
};

#endif