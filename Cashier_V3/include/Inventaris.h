#ifndef INVENTARIS_H
#define INVENTARIS_H

#include "Produk.h"
#include <vector>
#include <string>

class Inventaris {
    std::vector<Produk> daftarProduk;
    std::string pathFile;

    void muatDariFile();
    void simpanKeFile() const;
    void buatDataAwal();

public:
        explicit Inventaris(const std::string& filePath);

        void tampilkanKatalog() const;
        void cekStokMenipis(int batasMinimum = 5) const;
        void cariProduk(const std::string& kataKunci) const;

        Produk* cariBerdasarkanKode(const std::string& kode);
        bool tambahProdukBaru(const Produk& produkBaru);
        bool ubahHargaProduk(const std::string& kode, double hargaBaru);
        bool tambahStokProduk(const std::string& kode, int tambahanStok);
        bool hapusProduk(const std::string& kode);
        void kurangiStokSetelahTransaksi(const std::vector<ItemKeranjang>& keranjang);

};

#endif