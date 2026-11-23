#ifndef PRODUK_H
#define PRODUK_H

#include <string>

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

#endif