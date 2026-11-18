#include "../include/kasir.h"
#include <iostream>
#include <iomanip>
#include <limits>

void tampilkanMenu() {
    std::cout << "\n=== APLIKASI KASIR CLI ===\n";
    std::cout << "1. Tambah Barang ke Keranjang\n";
    std::cout << "2. Lihat Keranjang\n";
    std::cout << "3. Proses Pembayaran & Cetak Struk\n";
    std::cout << "0. Keluar\n";
    std::cout << "Pilih menu: ";
}

void tambahBarang(std::vector<Barang>& keranjang) {
    Barang itemBaru;
    
    std::cout << "Nama barang: ";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Bersihkan buffer
    std::getline(std::cin, itemBaru.nama);

    // Validasi input angka untuk harga
    std::cout << "Harga satuan: Rp ";
    while (!(std::cin >> itemBaru.harga) || itemBaru.harga < 0) {
        std::cout << "Input tidak valid! Masukkan angka positif: Rp ";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }

    // Validasi input angka untuk jumlah
    std::cout << "Jumlah: ";
    while (!(std::cin >> itemBaru.jumlah) || itemBaru.jumlah <= 0) {
        std::cout << "Input tidak valid! Masukkan jumlah minimal 1: ";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }

    keranjang.push_back(itemBaru);
    std::cout << "-> '" << itemBaru.nama << "' berhasil ditambahkan!\n";
}

void tampilkanKeranjang(const std::vector<Barang>& keranjang) {
    if (keranjang.empty()) {
        std::cout << "\n[Keranjang masih kosong]\n";
        return;
    }

    double total = 0;
    std::cout << "\n--- DAFTAR BELANJA ---\n";
    std::cout << std::fixed << std::setprecision(0); // Format angka tanpa desimal

    for (size_t i = 0; i < keranjang.size(); ++i) {
        double subtotal = keranjang[i].harga * keranjang[i].jumlah;
        total += subtotal;
        std::cout << i + 1 << ". " << keranjang[i].nama 
                  << " (" << keranjang[i].jumlah << " x Rp " << keranjang[i].harga << ")"
                  << " = Rp " << subtotal << "\n";
    }
    std::cout << "----------------------\n";
    std::cout << "Total Sementara: Rp " << total << "\n";
}

void prosesPembayaran(std::vector<Barang>& keranjang) {
    if (keranjang.empty()) {
        std::cout << "\n[Tidak ada barang untuk dibayar]\n";
        return;
    }

    tampilkanKeranjang(keranjang);
    
    double total = 0;
    for (const auto& item : keranjang) {
        total += item.harga * item.jumlah;
    }

    double uangBayar = 0;
    std::cout << "Masukkan uang pembayaran: Rp ";
    while (!(std::cin >> uangBayar) || uangBayar < total) {
        std::cout << "Uang kurang atau input salah! Minimal Rp " << total << ": Rp ";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }

    double kembalian = uangBayar - total;
    std::cout << "\n=== STRUK PEMBAYARAN ===\n";
    std::cout << "Total Belanja : Rp " << total << "\n";
    std::cout << "Tunai         : Rp " << uangBayar << "\n";
    std::cout << "Kembalian     : Rp " << kembalian << "\n";
    std::cout << "========================\n";

    keranjang.clear(); // Kosongkan keranjang setelah transaksi selesai
}