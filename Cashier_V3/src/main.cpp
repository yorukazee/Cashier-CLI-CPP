#include "../include/Inventaris.h"
#include "../include/MesinKasir.h"

int main() {
    Inventaris gudang("data/produk.csv");
    MesinKasir kasir(gudang, "data/riwayat.csv");

    kasir.jalankan();

    return 0;
}