#include "../include/MesinKasir.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <limits>
#include <ctime>

MesinKasir::MesinKasir(Inventaris& inv, const std::string& fileRiwayat)
    : inventaris(inv), pathRiwayat(fileRiwayat) {}

void MesinKasir::jalankan() {
    int pilihan = -1;
    do {
        std::cout << "\n====================================\n";
        std::cout << "     SISTEM KASIR & STOK v3.0       \n";
        std::cout << "====================================\n";
        inventaris.cekStokMenipis(5);
        std::cout << "\n1. Masuk Mode Kasir (Transaksi)\n";
        std::cout << "2. Masuk Mode Admin (Kelola Stok & Laporan)\n";
        std::cout << "0. Keluar Aplikasi\n";
        std::cout << "------------------------------------\n";
        std::cout << "Pilih mode: ";

        if (!(std::cin >> pilihan)) {
            std::cout << "Input tidak valid! Masukkan angka.\n";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            continue;
        }

        switch (pilihan) {
            case 1:
                menuModeKasir();
                break;
            case 2:
                menuModeAdmin();
                break;
            case 0:
                std::cout << "Menyimpan data & menutup aplikasi. Sampai jumpa!\n";
                break;
            default:
                std::cout << "Pilihan tidak tersedia.\n";
        }
    } while (pilihan != 0);
}

void MesinKasir::menuModeKasir() {
    int pilihan = -1;
    do {
        std::cout << "\n--- MODE KASIR ---\n";
        std::cout << "1. Lihat Katalog Barang\n";
        std::cout << "2. Cari Barang\n";
        std::cout << "3. Tambah Barang ke Keranjang (via Kode)\n";
        std::cout << "4. Lihat Keranjang\n";
        std::cout << "5. Edit Jumlah di Keranjang\n";
        std::cout << "6. Hapus Barang dari Keranjang\n";
        std::cout << "7. Proses Pembayaran & Cetak Struk\n";
        std::cout << "0. Kembali ke Menu Utama\n";
        std::cout << "Pilih aksi: ";

        if (!(std::cin >> pilihan)) {
            std::cout << "Input tidak valid!\n";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            continue;
        }

        switch (pilihan) {
            case 1: inventaris.tampilkanKatalog(); break;
            case 2: {
                std::string keyword;
                std::cout << "Masukkan kata kunci nama/kode: ";
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::getline(std::cin, keyword);
                inventaris.cariProduk(keyword);
                break;
            }
            case 3: tambahKeKeranjang(); break;
            case 4: tampilkanKeranjang(); break;
            case 5: editJumlahKeranjang(); break;
            case 6: hapusDariKeranjang(); break;
            case 7: prosesPembayaran(); break;
            case 0: break;
            default: std::cout << "Pilihan tidak tersedia.\n";
        }
    } while (pilihan != 0);
}

void MesinKasir::menuModeAdmin() {
    std::string inputPin;
    std::cout << "\nMasukkan PIN Admin (default: 1234): ";
    std::cin >> inputPin;

    if (inputPin != PIN_ADMIN) {
        std::cout << "[Akses Ditolak: PIN salah!]\n";
        return;
    }

    int pilihan = -1;
    do {
        std::cout << "\n--- MODE ADMIN & GUDANG ---\n";
        std::cout << "1. Lihat Katalog & Stok\n";
        std::cout << "2. Tambah Produk Baru ke Katalog\n";
        std::cout << "3. Restock Barang (Tambah Stok)\n";
        std::cout << "4. Ubah Harga Barang\n";
        std::cout << "5. Hapus Produk dari Katalog\n";
        std::cout << "6. Laporan Riwayat Penjualan\n";
        std::cout << "0. Kembali ke Menu Utama\n";
        std::cout << "Pilih aksi: ";

        if (!(std::cin >> pilihan)) {
            std::cout << "Input tidak valid!\n";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            continue;
        }

        switch (pilihan) {
            case 1: inventaris.tampilkanKatalog(); break;
            case 2: {
                Produk baru;
                std::cout << "Kode Barang (misal BRG06): ";
                std::cin >> baru.kode;
                std::cout << "Nama Barang (hindari tanda koma): ";
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::getline(std::cin, baru.nama);

                std::cout << "Harga Satuan: Rp ";
                while (!(std::cin >> baru.harga) || baru.harga < 0) {
                    std::cout << "Masukkan harga valid: Rp ";
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                }

                std::cout << "Stok Awal: ";
                while (!(std::cin >> baru.stok) || baru.stok < 0) {
                    std::cout << "Masukkan stok valid: ";
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                }

                if (inventaris.tambahProdukBaru(baru)) {
                    std::cout << "-> Produk berhasil disimpan ke database!\n";
                } else {
                    std::cout << "[Gagal: Kode barang '" << baru.kode << "' sudah terdaftar!]\n";
                }
                break;
            }
            case 3: {
                inventaris.tampilkanKatalog();
                std::string kode;
                int tambah;
                std::cout << "Masukkan Kode Barang: ";
                std::cin >> kode;
                std::cout << "Jumlah stok masuk: ";
                while (!(std::cin >> tambah) || tambah <= 0) {
                    std::cout << "Masukkan angka minimal 1: ";
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                }
                if (inventaris.tambahStokProduk(kode, tambah)) {
                    std::cout << "-> Stok berhasil diperbarui!\n";
                } else {
                    std::cout << "[Gagal: Kode barang tidak ditemukan!]\n";
                }
                break;
            }
            case 4: {
                inventaris.tampilkanKatalog();
                std::string kode;
                double hargaBaru;
                std::cout << "Masukkan Kode Barang: ";
                std::cin >> kode;
                std::cout << "Masukkan Harga Baru: Rp ";
                while (!(std::cin >> hargaBaru) || hargaBaru < 0) {
                    std::cout << "Masukkan harga valid: Rp ";
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                }
                if (inventaris.ubahHargaProduk(kode, hargaBaru)) {
                    std::cout << "-> Harga berhasil diperbarui!\n";
                } else {
                    std::cout << "[Gagal: Kode barang tidak ditemukan!]\n";
                }
                break;
            }
            case 5: {
                inventaris.tampilkanKatalog();
                std::string kode;
                std::cout << "Masukkan Kode Barang yang akan dihapus: ";
                std::cin >> kode;
                if (inventaris.hapusProduk(kode)) {
                    std::cout << "-> Produk berhasil dihapus dari database!\n";
                } else {
                    std::cout << "[Gagal: Kode barang tidak ditemukan!]\n";
                }
                break;
            }
            case 6: tampilkanLaporanPenjualan(); break;
            case 0: break;
            default: std::cout << "Pilihan tidak tersedia.\n";
        }
    } while (pilihan != 0);
}

void MesinKasir::tambahKeKeranjang() {
    inventaris.tampilkanKatalog();
    std::string kode;
    std::cout << "Masukkan Kode Barang: ";
    std::cin >> kode;

    Produk* produk = inventaris.cariBerdasarkanKode(kode);
    if (produk == nullptr) {
        std::cout << "[Kode barang '" << kode << "' tidak ditemukan!]\n";
        return;
    }

    if (produk->stok <= 0) {
        std::cout << "[Stok '" << produk->nama << "' habis!]\n";
        return;
    }

    int sudahDiKeranjang = 0;
    for (const auto& item : keranjang) {
        if (item.kode == kode) {
            sudahDiKeranjang = item.jumlah;
            break;
        }
    }

    int sisaTersedia = produk->stok - sudahDiKeranjang;
    if (sisaTersedia <= 0) {
        std::cout << "[Seluruh stok yang tersedia sudah masuk di keranjang!]\n";
        return;
    }

    int qty;
    std::cout << "Jumlah beli (Tersedia: " << sisaTersedia << "): ";
    while (!(std::cin >> qty) || qty <= 0 || qty > sisaTersedia) {
        std::cout << "Jumlah tidak valid! Masukkan 1 - " << sisaTersedia << ": ";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }

    for (auto& item : keranjang) {
        if (item.kode == kode) {
            item.jumlah += qty;
            std::cout << "-> Jumlah '" << item.nama << "' diperbarui menjadi " << item.jumlah << "!\n";
            return;
        }
    }

    keranjang.push_back({produk->kode, produk->nama, produk->harga, qty});
    std::cout << "-> '" << produk->nama << "' (" << qty << " pcs) masuk ke keranjang!\n";
}

void MesinKasir::tampilkanKeranjang() const {
    if (keranjang.empty()) {
        std::cout << "\n[Keranjang masih kosong]\n";
        return;
    }

    double total = 0;
    std::cout << "\n--- KERANJANG BELANJA ---\n";
    std::cout << std::fixed << std::setprecision(0);
    for (size_t i = 0; i < keranjang.size(); ++i) {
        double sub = keranjang[i].harga * keranjang[i].jumlah;
        total += sub;
        std::cout << i + 1 << ". [" << keranjang[i].kode << "] " << keranjang[i].nama
                  << " (" << keranjang[i].jumlah << " x Rp " << keranjang[i].harga << ")"
                  << " = Rp " << sub << "\n";
    }
    std::cout << "-------------------------\n";
    std::cout << "Subtotal Sementara: Rp " << total << "\n";
}

void MesinKasir::editJumlahKeranjang() {
    if (keranjang.empty()) {
        std::cout << "\n[Keranjang kosong]\n";
        return;
    }
    tampilkanKeranjang();

    int nomor;
    std::cout << "Pilih nomor item yang diubah (0 batal): ";
    while (!(std::cin >> nomor) || nomor < 0 || nomor > static_cast<int>(keranjang.size())) {
        std::cout << "Nomor tidak valid: ";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
    if (nomor == 0) return;

    ItemKeranjang& item = keranjang[nomor - 1];
    Produk* produk = inventaris.cariBerdasarkanKode(item.kode);
    int maksStok = (produk != nullptr) ? produk->stok : item.jumlah;

    int qtyBaru;
    std::cout << "Jumlah baru untuk '" << item.nama << "' (Maks " << maksStok << "): ";
    while (!(std::cin >> qtyBaru) || qtyBaru <= 0 || qtyBaru > maksStok) {
        std::cout << "Masukkan 1 - " << maksStok << ": ";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }

    item.jumlah = qtyBaru;
    std::cout << "-> Jumlah berhasil diubah!\n";
}

void MesinKasir::hapusDariKeranjang() {
    if (keranjang.empty()) {
        std::cout << "\n[Keranjang kosong]\n";
        return;
    }
    tampilkanKeranjang();

    int nomor;
    std::cout << "Pilih nomor item yang dihapus (0 batal): ";
    while (!(std::cin >> nomor) || nomor < 0 || nomor > static_cast<int>(keranjang.size())) {
        std::cout << "Nomor tidak valid: ";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
    if (nomor == 0) return;

    std::cout << "-> '" << keranjang[nomor - 1].nama << "' dihapus dari keranjang.\n";
    keranjang.erase(keranjang.begin() + (nomor - 1));
}

void MesinKasir::cetakStrukKeFile(double subtotal, double diskon, double totalAkhir, double tunai, double kembalian) const {
    std::time_t now = std::time(nullptr);
    char namaFile[100];
    char waktuStr[100];
    std::strftime(namaFile, sizeof(namaFile), "data/struk_%Y%m%d_%H%M%S.txt", std::localtime(&now));
    std::strftime(waktuStr, sizeof(waktuStr), "%d-%m-%Y %H:%M:%S", std::localtime(&now));

    std::ofstream file(namaFile);
    if (!file.is_open()) return;

    file << std::fixed << std::setprecision(0);
    file << "====================================\n";
    file << "         STRUK PEMBAYARAN           \n";
    file << "Waktu: " << waktuStr << "\n";
    file << "====================================\n";
    for (size_t i = 0; i < keranjang.size(); ++i) {
        file << i + 1 << ". " << keranjang[i].nama << "\n"
             << "   " << keranjang[i].jumlah << " x Rp " << keranjang[i].harga
             << " = Rp " << (keranjang[i].harga * keranjang[i].jumlah) << "\n";
    }
    file << "------------------------------------\n";
    file << "Subtotal      : Rp " << subtotal << "\n";
    if (diskon > 0) file << "Diskon (10%)  : -Rp " << diskon << "\n";
    file << "Total Akhir   : Rp " << totalAkhir << "\n";
    file << "Tunai         : Rp " << tunai << "\n";
    file << "Kembalian     : Rp " << kembalian << "\n";
    file << "====================================\n";
    file.close();

    std::cout << "-> Struk disimpan ke '" << namaFile << "'\n";
}

void MesinKasir::catatRiwayatTransaksi(int totalItem, double totalAkhir) const {
    std::ofstream file(pathRiwayat, std::ios::app); // Mode append (menambah baris baru)
    if (!file.is_open()) return;

    std::time_t now = std::time(nullptr);
    char waktuStr[100];
    std::strftime(waktuStr, sizeof(waktuStr), "%d-%m-%Y %H:%M:%S", std::localtime(&now));

    file << std::fixed << std::setprecision(0);
    file << waktuStr << "," << totalItem << "," << totalAkhir << "\n";
    file.close();
}

void MesinKasir::tampilkanLaporanPenjualan() const {
    std::ifstream file(pathRiwayat);
    if (!file.is_open()) {
        std::cout << "\n[Belum ada riwayat transaksi yang tercatat]\n";
        return;
    }

    std::string baris;
    int jumlahTransaksi = 0;
    int totalBarangTerjual = 0;
    double totalOmzet = 0;

    std::cout << "\n=================================================\n";
    std::cout << "            LAPORAN RIWAYAT PENJUALAN            \n";
    std::cout << "=================================================\n";
    std::cout << std::fixed << std::setprecision(0);

    while (std::getline(file, baris)) {
        if (baris.empty()) continue;
        std::stringstream ss(baris);
        std::string waktu, itemStr, totalStr;

        if (std::getline(ss, waktu, ',') &&
            std::getline(ss, itemStr, ',') &&
            std::getline(ss, totalStr, ',')) {
            int qty = std::stoi(itemStr);
            double nominal = std::stod(totalStr);

            jumlahTransaksi++;
            totalBarangTerjual += qty;
            totalOmzet += nominal;

            std::cout << jumlahTransaksi << ". [" << waktu << "] "
                      << qty << " item | Total: Rp " << nominal << "\n";
        }
    }
    file.close();

    std::cout << "-------------------------------------------------\n";
    std::cout << "Total Transaksi      : " << jumlahTransaksi << " transaksi\n";
    std::cout << "Total Barang Terjual : " << totalBarangTerjual << " pcs\n";
    std::cout << "Total Pendapatan     : Rp " << totalOmzet << "\n";
    std::cout << "=================================================\n";
}

void MesinKasir::prosesPembayaran() {
    if (keranjang.empty()) {
        std::cout << "\n[Keranjang kosong, tidak ada transaksi]\n";
        return;
    }

    tampilkanKeranjang();

    double subtotal = 0;
    int totalItem = 0;
    for (const auto& item : keranjang) {
        subtotal += item.harga * item.jumlah;
        totalItem += item.jumlah;
    }

    double diskon = (subtotal >= 100000) ? subtotal * 0.10 : 0;
    if (diskon > 0) {
        std::cout << "[Diskon 10% diterapkan: -Rp " << diskon << "]\n";
    }

    double totalAkhir = subtotal - diskon;
    std::cout << "Total Akhir: Rp " << totalAkhir << "\n";

    double tunai = 0;
    std::cout << "Uang Tunai : Rp ";
    while (!(std::cin >> tunai) || tunai < totalAkhir) {
        std::cout << "Uang kurang! Minimal Rp " << totalAkhir << ": Rp ";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }

    double kembalian = tunai - totalAkhir;
    std::cout << "Kembalian  : Rp " << kembalian << "\n";

    // 1. Kurangi stok di database & simpan ke produk.csv
    inventaris.kurangiStokSetelahTransaksi(keranjang);

    // 2. Cetak struk unik berdasarkan jam transaksi
    cetakStrukKeFile(subtotal, diskon, totalAkhir, tunai, kembalian);

    // 3. Catat omzet ke riwayat.csv
    catatRiwayatTransaksi(totalItem, totalAkhir);

    keranjang.clear();
    std::cout << "-> Transaksi selesai & stok database telah diperbarui!\n";
}