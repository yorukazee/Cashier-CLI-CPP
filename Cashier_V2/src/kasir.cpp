#include "../include/kasir.h"
#include <iostream>
#include <iomanip>
#include <limits>
#include <fstream>
#include <ctime>

void tampilkanMenu() {
    std::cout << "\n====================================\n";
    std::cout << "       APLIKASI KASIR CLI v2.0      \n";
    std::cout << "====================================\n";
    std::cout << "1. Tambah Barang ke Keranjang\n";
    std::cout << "2. Lihat Keranjang\n";
    std::cout << "3. Edit Jumlah Barang\n";
    std::cout << "4. Hapus Barang dari Keranjang\n";
    std::cout << "5. Proses Pembayaran & Cetak Struk\n";
    std::cout << "0. Keluar\n";
    std::cout << "------------------------------------\n";
    std::cout << "Pilih menu: ";
}

void tambahBarang(std::vector<Barang>& keranjang) {
    Barang itemBaru;

    std::cout << "\nNama barang: ";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::getline(std::cin, itemBaru.nama);

    std::cout << "Harga satuan: Rp ";
    while (!(std::cin >> itemBaru.harga) || itemBaru.harga < 0) {
        std::cout << "Input tidak valid! Masukkan angka positif: Rp ";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }

    std::cout << "Jumlah: ";
    while (!(std::cin >> itemBaru.jumlah) || itemBaru.jumlah <= 0) {
        std::cout << "Input tidak valid! Masukkan jumlah minimal 1: ";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }

    // Jika nama barang sudah ada di keranjang, gabungkan jumlahnya (jika harga sama)
    for (auto& item : keranjang) {
        if (item.nama == itemBaru.nama && item.harga == itemBaru.harga) {
            item.jumlah += itemBaru.jumlah;
            std::cout << "-> Jumlah '" << item.nama << "' diperbarui menjadi " << item.jumlah << "!\n";
            return;
        }
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
    std::cout << std::fixed << std::setprecision(0);

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

void editJumlahBarang(std::vector<Barang>& keranjang) {
    if (keranjang.empty()) {
        std::cout << "\n[Keranjang kosong, tidak ada yang bisa diubah]\n";
        return;
    }

    tampilkanKeranjang(keranjang);

    int nomor;
    std::cout << "Masukkan nomor barang yang ingin diubah (0 untuk batal): ";
    while (!(std::cin >> nomor) || nomor < 0 || nomor > static_cast<int>(keranjang.size())) {
        std::cout << "Nomor tidak valid! Masukkan angka 0 - " << keranjang.size() << ": ";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }

    if (nomor == 0) {
        std::cout << "Perubahan dibatalkan.\n";
        return;
    }

    int jumlahBaru;
    std::cout << "Masukkan jumlah baru untuk '" << keranjang[nomor - 1].nama << "': ";
    while (!(std::cin >> jumlahBaru) || jumlahBaru <= 0) {
        std::cout << "Input tidak valid! Masukkan jumlah minimal 1: ";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }

    keranjang[nomor - 1].jumlah = jumlahBaru;
    std::cout << "-> Jumlah '" << keranjang[nomor - 1].nama << "' berhasil diubah menjadi " << jumlahBaru << "!\n";
}

void hapusBarang(std::vector<Barang>& keranjang) {
    if (keranjang.empty()) {
        std::cout << "\n[Keranjang kosong, tidak ada yang bisa dihapus]\n";
        return;
    }

    tampilkanKeranjang(keranjang);

    int nomor;
    std::cout << "Masukkan nomor barang yang ingin dihapus (0 untuk batal): ";
    while (!(std::cin >> nomor) || nomor < 0 || nomor > static_cast<int>(keranjang.size())) {
        std::cout << "Nomor tidak valid! Masukkan angka 0 - " << keranjang.size() << ": ";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }

    if (nomor == 0) {
        std::cout << "Penghapusan dibatalkan.\n";
        return;
    }

    std::string namaDihapus = keranjang[nomor - 1].nama;
    keranjang.erase(keranjang.begin() + (nomor - 1));
    std::cout << "-> '" << namaDihapus << "' berhasil dihapus dari keranjang!\n";
}

void simpanStrukKeFile(
    const std::vector<Barang>& keranjang,
    double subtotal,
    double diskon,
    double totalAkhir,
    double uangBayar,
    double kembalian
) {
    std::ofstream fileStruk("struk.txt");
    if (!fileStruk.is_open()) {
        std::cout << "[Peringatan: Gagal menyimpan file struk.txt]\n";
        return;
    }

    std::time_t sekarang = std::time(nullptr);
    char waktuStr[100];
    std::strftime(waktuStr, sizeof(waktuStr), "%d-%m-%Y %H:%M:%S", std::localtime(&sekarang));

    fileStruk << std::fixed << std::setprecision(0);
    fileStruk << "====================================\n";
    fileStruk << "         STRUK PEMBAYARAN           \n";
    fileStruk << "Waktu: " << waktuStr << "\n";
    fileStruk << "====================================\n";

    for (size_t i = 0; i < keranjang.size(); ++i) {
        double itemTotal = keranjang[i].harga * keranjang[i].jumlah;
        fileStruk << i + 1 << ". " << keranjang[i].nama << "\n"
                  << "   " << keranjang[i].jumlah << " x Rp " << keranjang[i].harga
                  << " = Rp " << itemTotal << "\n";
    }

    fileStruk << "------------------------------------\n";
    fileStruk << "Subtotal      : Rp " << subtotal << "\n";
    if (diskon > 0) {
        fileStruk << "Diskon (10%)  : -Rp " << diskon << "\n";
    }
    fileStruk << "Total Akhir   : Rp " << totalAkhir << "\n";
    fileStruk << "Tunai         : Rp " << uangBayar << "\n";
    fileStruk << "Kembalian     : Rp " << kembalian << "\n";
    fileStruk << "====================================\n";
    fileStruk << "    Terima Kasih Telah Berbelanja   \n";

    fileStruk.close();
    std::cout << "-> Struk berhasil dicetak ke file 'struk.txt'!\n";
}

void prosesPembayaran(std::vector<Barang>& keranjang) {
    if (keranjang.empty()) {
        std::cout << "\n[Tidak ada barang untuk dibayar]\n";
        return;
    }

    tampilkanKeranjang(keranjang);

    double subtotal = 0;
    for (const auto& item : keranjang) {
        subtotal += item.harga * item.jumlah;
    }

    double diskon = 0;
    if (subtotal >= 100000) {
        diskon = subtotal * 0.10;
        std::cout << "\n[Selamat! Anda mendapat diskon 10% sebesar Rp " << diskon << "]\n";
    }

    double totalAkhir = subtotal - diskon;
    std::cout << "Total yang harus dibayar: Rp " << totalAkhir << "\n";

    double uangBayar = 0;
    std::cout << "Masukkan uang pembayaran: Rp ";
    while (!(std::cin >> uangBayar) || uangBayar < totalAkhir) {
        std::cout << "Uang kurang atau input salah! Minimal Rp " << totalAkhir << ": Rp ";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }

    double kembalian = uangBayar - totalAkhir;
    std::cout << "\n=== STRUK PEMBAYARAN ===\n";
    std::cout << "Subtotal      : Rp " << subtotal << "\n";
    if (diskon > 0) {
        std::cout << "Diskon (10%)  : -Rp " << diskon << "\n";
    }
    std::cout << "Total Akhir   : Rp " << totalAkhir << "\n";
    std::cout << "Tunai         : Rp " << uangBayar << "\n";
    std::cout << "Kembalian     : Rp " << kembalian << "\n";
    std::cout << "========================\n";

    simpanStrukKeFile(keranjang, subtotal, diskon, totalAkhir, uangBayar, kembalian);
    keranjang.clear();
}