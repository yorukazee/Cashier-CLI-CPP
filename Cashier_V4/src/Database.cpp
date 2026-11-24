#include "../include/Database.h"
#include "../include/UIHelper.h"
#include <filesystem>
#include <functional>

namespace fs = std::filesystem;

Database::Database(const std::string& path) : db(nullptr), dbPath(path) {
    fs::path p(dbPath);
    if (p.has_parent_path() && !fs::exists(p.parent_path())) {
        fs::create_directories(p.parent_path());
    }

    if (sqlite3_open(dbPath.c_str(), &db) != SQLITE_OK) {
        UI::cetakError("Gagal membuka database SQLite: " + std::string(sqlite3_errmsg(db)));
        db = nullptr;
        return;
    }

    // Aktifkan Foreign Key di SQLite
    eksekusiSQL("PRAGMA foreign_keys = ON;");
    inisialisasiTabel();
    isiDataAwalJikaKosong();
}

Database::~Database() {
    if (db != nullptr) {
        sqlite3_close(db);
    }
}

bool Database::eksekusiSQL(const std::string& sql) const {
    if (db == nullptr) return false;
    char* errMsg = nullptr;
    int rc = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        UI::cetakError("SQL Error: " + std::string(errMsg));
        sqlite3_free(errMsg);
        return false;
    }
    return true;
}

void Database::inisialisasiTabel() {
    eksekusiSQL(
        "CREATE TABLE IF NOT EXISTS users ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "username TEXT UNIQUE NOT NULL,"
        "password_hash TEXT NOT NULL,"
        "role TEXT NOT NULL,"
        "nama_lengkap TEXT NOT NULL);"
    );

    eksekusiSQL(
        "CREATE TABLE IF NOT EXISTS produk ("
        "kode TEXT PRIMARY KEY,"
        "nama TEXT NOT NULL,"
        "harga REAL NOT NULL,"
        "stok INTEGER NOT NULL);"
    );

    eksekusiSQL(
        "CREATE TABLE IF NOT EXISTS promo ("
        "kode_promo TEXT PRIMARY KEY,"
        "persen_diskon REAL NOT NULL,"
        "aktif INTEGER NOT NULL DEFAULT 1);"
    );

    eksekusiSQL(
        "CREATE TABLE IF NOT EXISTS pengaturan ("
        "kunci TEXT PRIMARY KEY,"
        "nilai TEXT NOT NULL);"
    );

    eksekusiSQL(
        "CREATE TABLE IF NOT EXISTS transaksi ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "tanggal TEXT NOT NULL,"
        "waktu TEXT NOT NULL,"
        "kasir_username TEXT NOT NULL,"
        "kasir_nama TEXT NOT NULL,"
        "subtotal REAL NOT NULL,"
        "diskon_promo REAL NOT NULL,"
        "pajak_ppn REAL NOT NULL,"
        "total_akhir REAL NOT NULL);"
    );

    eksekusiSQL(
        "CREATE TABLE IF NOT EXISTS detail_transaksi ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "transaksi_id INTEGER NOT NULL,"
        "kode_produk TEXT NOT NULL,"
        "nama_produk TEXT NOT NULL,"
        "harga_satuan REAL NOT NULL,"
        "jumlah INTEGER NOT NULL,"
        "FOREIGN KEY(transaksi_id) REFERENCES transaksi(id));"
    );
}

void Database::isiDataAwalJikaKosong() {
    sqlite3_stmt* stmt;
    int jumlahUser = 0;
    if (sqlite3_prepare_v2(db, "SELECT COUNT(*) FROM users;", -1, &stmt, nullptr) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            jumlahUser = sqlite3_column_int(stmt, 0);
        }
        sqlite3_finalize(stmt);
    }

    if (jumlahUser == 0) {
        std::string hashAdmin = std::to_string(std::hash<std::string>{}("admin123"));
        std::string hashKasir = std::to_string(std::hash<std::string>{}("kasir123"));

        eksekusiSQL("INSERT INTO users (username, password_hash, role, nama_lengkap) VALUES "
                    "('admin', '" + hashAdmin + "', 'admin', 'Administrator Toko'),"
                    "('kasir1', '" + hashKasir + "', 'kasir', 'Kasir Shift Pagi');");
    }

    int jumlahProduk = 0;
    if (sqlite3_prepare_v2(db, "SELECT COUNT(*) FROM produk;", -1, &stmt, nullptr) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            jumlahProduk = sqlite3_column_int(stmt, 0);
        }
        sqlite3_finalize(stmt);
    }

    if (jumlahProduk == 0) {
        eksekusiSQL("INSERT INTO produk (kode, nama, harga, stok) VALUES "
                    "('BRG01', 'Kopi Arabika 250g', 45000, 15),"
                    "('BRG02', 'Teh Hijau Celup', 18000, 25),"
                    "('BRG03', 'Gula Aren Organik', 28000, 4),"
                    "('BRG04', 'Susu UHT Full Cream', 19500, 20),"
                    "('BRG05', 'Roti Gandum Utuh', 22000, 3);");
    }

    eksekusiSQL("INSERT OR IGNORE INTO promo (kode_promo, persen_diskon, aktif) VALUES "
                "('HEMAT10', 10.0, 1),"
                "('PROMO20', 20.0, 1);");

    eksekusiSQL("INSERT OR IGNORE INTO pengaturan (kunci, nilai) VALUES "
                "('ppn_aktif', '1'),"
                "('tarif_ppn', '11');");
}