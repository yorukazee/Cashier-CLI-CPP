#include "../include/Inventaris.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <filesystem>

namespace fs = std::filesystem;

Inventaris::Inventaris(const std::string& filePath) : pathFile(filePath) {
    fs::path p(pathFile);
    if (p.has_parent_path() && !fs::exists(p.parent_path())) {
        fs::create_directories(p.parent_path());
    }
    muatDariFile();
}

void Inventaris::buatDataAwal() {
    daftarProduk = {
        {"BRG01", "Kopi Arabika 250g", 45000, 15},
        {"BRG02", "Teh Hijau Celup", 18000, 25},
        {"BRG03", "Gula Aren Organik", 28000, 4},
        {"BRG04", "Susu UHT Full Cream", 19500, 20},
        {"BRG05", "Roti Gandum Utuh", 22000, 3}
    };
    simpanKeFile();
}

void Inventaris::muatDariFile() {
    std::ifstream file(pathFile);
    if (!file.is_open()) {
        buatDataAwal();
        return;
    }

    daftarProduk.clear();
    std::string baris;
    while (std::getline(file, baris)) {
        if (baris.empty()) continue;

        std::stringstream ss(baris);
        std::string kode, nama, hargaStr, stokStr;

        if (std::getline(ss, kode, ',') &&
            std::getline(ss, nama, ',') &&
            std::getline(ss, hargaStr, ',') &&
            std::getline(ss, stokStr, ',')) {
            try {
                Produk p{kode, nama, std::stod(hargaStr), std::stoi(stokStr)};
                daftarProduk.push_back(p);
            } catch (...) {
                continue;
            }
        }
    }
    file.close();
}

void Inventaris::simpanKeFile() const {
    std::ofstream file(pathFile);
    if (!file.is_open()) {
        std::cout << "[Error: Gagal menyimpan database ke " << pathFile << "]\n";
        return;
    }

    for (const auto& p : daftarProduk) {
        file << p.kode << "," << p.nama << "," << p.harga << "," << p.stok << "\n";
    }
    file.close();
}

void Inventaris::tampilkanKatalog() const {
    if (daftarProduk.empty()) {
        std::cout << "\n[Katalog produk masih kosong]\n";
        return;
    }

    std::cout << "\n==========================================================\n";
    std::cout << std::left << std::setw(10) << "KODE"
              << std::setw(25) << "NAMA BARANG"
              << std::setw(15) << "HARGA"
              << std::setw(8) << "STOK" << "\n";
    std::cout << "==========================================================\n";
    std::cout << std::fixed << std::setprecision(0);

    for (const auto& p : daftarProduk) {
        std::cout << std::left << std::setw(10) << p.kode
                  << std::setw(25) << p.nama
                  << "Rp " << std::setw(12) << p.harga
                  << std::setw(8) << p.stok << "\n";
    }
    std::cout << "==========================================================\n";
}

void Inventaris::cekStokMenipis(int batasMinimum) const {
    bool adaMenipis = false;
    for (const auto& p : daftarProduk) {
        if (p.stok < batasMinimum) {
            if (!adaMenipis) {
                std::cout << "\n[!] PERINGATAN STOK MENIPIS (< " << batasMinimum << "):\n";
                adaMenipis = true;
            }
            std::cout << "    - " << p.kode << " (" << p.nama << ") : Sisa " << p.stok << "\n";
        }
    }
}

void Inventaris::cariProduk(const std::string& kataKunci) const {
    std::string keyLower = kataKunci;
    std::transform(keyLower.begin(), keyLower.end(), keyLower.begin(), ::tolower);

    bool ditemukan = false;
    std::cout << "\n--- HASIL PENCARIAN '" << kataKunci << "' ---\n";
    std::cout << std::fixed << std::setprecision(0);

    for (const auto& p : daftarProduk) {
        std::string kodeLower = p.kode;
        std::string namaLower = p.nama;
        std::transform(kodeLower.begin(), kodeLower.end(), kodeLower.begin(), ::tolower);
        std::transform(namaLower.begin(), namaLower.end(), namaLower.begin(), ::tolower);

        if (kodeLower.find(keyLower) != std::string::npos || namaLower.find(keyLower) != std::string::npos) {
            std::cout << "[" << p.kode << "] " << p.nama << " - Rp " << p.harga << " (Stok: " << p.stok << ")\n";
            ditemukan = true;
        }
    }

    if (!ditemukan) {
        std::cout << "Produk tidak ditemukan.\n";
    }
}

Produk* Inventaris::cariBerdasarkanKode(const std::string& kode) {
    for (auto& p : daftarProduk) {
        if (p.kode == kode) {
            return &p;
        }
    }
    return nullptr;
}

bool Inventaris::tambahProdukBaru(const Produk& produkBaru) {
    if (cariBerdasarkanKode(produkBaru.kode) != nullptr) {
        return false; // Kode duplikat
    }
    daftarProduk.push_back(produkBaru);
    simpanKeFile();
    return true;
}

bool Inventaris::ubahHargaProduk(const std::string& kode, double hargaBaru) {
    Produk* p = cariBerdasarkanKode(kode);
    if (p == nullptr) return false;

    p->harga = hargaBaru;
    simpanKeFile();
    return true;
}

bool Inventaris::tambahStokProduk(const std::string& kode, int tambahanStok) {
    Produk* p = cariBerdasarkanKode(kode);
    if (p == nullptr) return false;

    p->stok += tambahanStok;
    simpanKeFile();
    return true;
}

bool Inventaris::hapusProduk(const std::string& kode) {
    for (size_t i = 0; i < daftarProduk.size(); ++i) {
        if (daftarProduk[i].kode == kode) {
            daftarProduk.erase(daftarProduk.begin() + i);
            simpanKeFile();
            return true;
        }
    }
    return false;
}

void Inventaris::kurangiStokSetelahTransaksi(const std::vector<ItemKeranjang>& keranjang) {
    for (const auto& item : keranjang) {
        Produk* p = cariBerdasarkanKode(item.kode);
        if (p != nullptr) {
            p->stok = std::max(0, p->stok - item.jumlah);
        }
    }
    simpanKeFile();
}