#include "../include/Inventaris.h"
#include "../include/UIHelper.h"
#include <iostream>
#include <iomanip>

Inventaris::Inventaris(Database& database) : db(database) {}

void Inventaris::tampilkanKatalog() const {
    const char* sql = "SELECT kode, nama, harga, stok FROM produk ORDER BY kode ASC;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db.getHandle(), sql, -1, &stmt, nullptr) != SQLITE_OK) return;

    std::cout << "\n" << UI::CYAN << "==========================================================\n";
    std::cout << std::left << std::setw(10) << "KODE"
              << std::setw(25) << "NAMA BARANG"
              << std::setw(15) << "HARGA"
              << std::setw(8) << "STOK" << "\n";
    std::cout << "==========================================================" << UI::RESET << "\n";
    std::cout << std::fixed << std::setprecision(0);

    bool adaData = false;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        adaData = true;
        std::string kode = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        std::string nama = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        double harga = sqlite3_column_double(stmt, 2);
        int stok = sqlite3_column_int(stmt, 3);

        std::string warnaStok = (stok < 5) ? UI::MERAH : UI::RESET;
        std::cout << std::left << std::setw(10) << kode
                  << std::setw(25) << nama
                  << "Rp " << std::setw(12) << harga
                  << warnaStok << std::setw(8) << stok << UI::RESET << "\n";
    }

    if (!adaData) {
        std::cout << "[Katalog produk masih kosong]\n";
    }
    std::cout << "==========================================================\n";
    sqlite3_finalize(stmt);
}

void Inventaris::cekStokMenipis(int batasMinimum) const {
    const char* sql = "SELECT kode, nama, stok FROM produk WHERE stok < ? ORDER BY stok ASC;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db.getHandle(), sql, -1, &stmt, nullptr) != SQLITE_OK) return;

    sqlite3_bind_int(stmt, 1, batasMinimum);

    bool pertama = true;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        if (pertama) {
            std::cout << "\n" << UI::MERAH << UI::BOLD << "[!] PERINGATAN STOK MENIPIS (< " << batasMinimum << "):" << UI::RESET << "\n";
            pertama = false;
        }
        std::string kode = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        std::string nama = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        int stok = sqlite3_column_int(stmt, 2);

        std::cout << UI::KUNING << "    - [" << kode << "] " << nama << " : Sisa " << stok << UI::RESET << "\n";
    }
    sqlite3_finalize(stmt);
}

void Inventaris::cariProduk(const std::string& kataKunci) const {
    const char* sql = "SELECT kode, nama, harga, stok FROM produk WHERE kode LIKE ? OR nama LIKE ? ORDER BY kode ASC;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db.getHandle(), sql, -1, &stmt, nullptr) != SQLITE_OK) return;

    std::string pola = "%" + kataKunci + "%";
    sqlite3_bind_text(stmt, 1, pola.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, pola.c_str(), -1, SQLITE_TRANSIENT);

    std::cout << "\n--- HASIL PENCARIAN '" << kataKunci << "' ---\n";
    std::cout << std::fixed << std::setprecision(0);

    bool ditemukan = false;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        ditemukan = true;
        std::cout << "[" << reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)) << "] "
                  << reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1))
                  << " - Rp " << sqlite3_column_double(stmt, 2)
                  << " (Stok: " << sqlite3_column_int(stmt, 3) << ")\n";
    }

    if (!ditemukan) {
        UI::cetakPeringatan("Produk tidak ditemukan.");
    }
    sqlite3_finalize(stmt);
}

std::optional<Produk> Inventaris::cariBerdasarkanKode(const std::string& kode) const {
    const char* sql = "SELECT kode, nama, harga, stok FROM produk WHERE kode = ?;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db.getHandle(), sql, -1, &stmt, nullptr) != SQLITE_OK) return std::nullopt;

    sqlite3_bind_text(stmt, 1, kode.c_str(), -1, SQLITE_TRANSIENT);

    std::optional<Produk> hasil = std::nullopt;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        Produk p;
        p.kode = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        p.nama = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        p.harga = sqlite3_column_double(stmt, 2);
        p.stok = sqlite3_column_int(stmt, 3);
        hasil = p;
    }
    sqlite3_finalize(stmt);
    return hasil;
}

bool Inventaris::tambahProdukBaru(const Produk& produkBaru) {
    const char* sql = "INSERT INTO produk (kode, nama, harga, stok) VALUES (?, ?, ?, ?);";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db.getHandle(), sql, -1, &stmt, nullptr) != SQLITE_OK) return false;

    sqlite3_bind_text(stmt, 1, produkBaru.kode.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, produkBaru.nama.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_double(stmt, 3, produkBaru.harga);
    sqlite3_bind_int(stmt, 4, produkBaru.stok);

    bool sukses = (sqlite3_step(stmt) == SQLITE_DONE);
    sqlite3_finalize(stmt);
    return sukses;
}

bool Inventaris::ubahHargaProduk(const std::string& kode, double hargaBaru) {
    const char* sql = "UPDATE produk SET harga = ? WHERE kode = ?;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db.getHandle(), sql, -1, &stmt, nullptr) != SQLITE_OK) return false;

    sqlite3_bind_double(stmt, 1, hargaBaru);
    sqlite3_bind_text(stmt, 2, kode.c_str(), -1, SQLITE_TRANSIENT);

    sqlite3_step(stmt);
    int berubah = sqlite3_changes(db.getHandle());
    sqlite3_finalize(stmt);
    return berubah > 0;
}

bool Inventaris::tambahStokProduk(const std::string& kode, int tambahanStok) {
    const char* sql = "UPDATE produk SET stok = stok + ? WHERE kode = ?;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db.getHandle(), sql, -1, &stmt, nullptr) != SQLITE_OK) return false;

    sqlite3_bind_int(stmt, 1, tambahanStok);
    sqlite3_bind_text(stmt, 2, kode.c_str(), -1, SQLITE_TRANSIENT);

    sqlite3_step(stmt);
    int berubah = sqlite3_changes(db.getHandle());
    sqlite3_finalize(stmt);
    return berubah > 0;
}

bool Inventaris::hapusProduk(const std::string& kode) {
    const char* sql = "DELETE FROM produk WHERE kode = ?;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db.getHandle(), sql, -1, &stmt, nullptr) != SQLITE_OK) return false;

    sqlite3_bind_text(stmt, 1, kode.c_str(), -1, SQLITE_TRANSIENT);

    sqlite3_step(stmt);
    int berubah = sqlite3_changes(db.getHandle());
    sqlite3_finalize(stmt);
    return berubah > 0;
}

bool Inventaris::kurangiStokSetelahTransaksi(const std::vector<ItemKeranjang>& keranjang) {
    const char* sql = "UPDATE produk SET stok = MAX(0, stok - ?) WHERE kode = ?;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db.getHandle(), sql, -1, &stmt, nullptr) != SQLITE_OK) return false;

    for (const auto& item : keranjang) {
        sqlite3_bind_int(stmt, 1, item.jumlah);
        sqlite3_bind_text(stmt, 2, item.kode.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_step(stmt);
        sqlite3_reset(stmt);
    }
    sqlite3_finalize(stmt);
    return true;
}