#include "../include/AuthManager.h"
#include "../include/UIHelper.h"
#include <functional>
#include <iostream>
#include <iomanip>

AuthManager::AuthManager(Database& database) : db(database) {}

std::string AuthManager::hashPassword(const std::string& password) const {
    return std::to_string(std::hash<std::string>{}(password));
}

bool AuthManager::login() {
    std::cout << "\n" << UI::CYAN << UI::BOLD << "=== LOGIN SISTEM POS v4.0 ===" << UI::RESET << "\n";
    std::cout << "(Default: admin / admin123 atau kasir1 / kasir123)\n";
    std::string user = UI::bacaBaris("Username : ");
    std::string pass = UI::bacaBaris("Password : ");

    std::string passHash = hashPassword(pass);
    const char* sql = "SELECT id, username, role, nama_lengkap FROM users WHERE username = ? AND password_hash = ?;";

    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db.getHandle(), sql, -1, &stmt, nullptr) != SQLITE_OK) {
        UI::cetakError("Gagal memproses query login.");
        return false;
    }

    sqlite3_bind_text(stmt, 1, user.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, passHash.c_str(), -1, SQLITE_TRANSIENT);

    bool berhasil = false;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        sesiSaatIni.id = sqlite3_column_int(stmt, 0);
        sesiSaatIni.username = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        sesiSaatIni.role = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        sesiSaatIni.namaLengkap = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        sesiSaatIni.sudahLogin = true;
        berhasil = true;
        UI::cetakSukses("Selamat datang, " + sesiSaatIni.namaLengkap + " [" + sesiSaatIni.role + "]!");
    } else {
        UI::cetakError("Username atau password salah!");
    }

    sqlite3_finalize(stmt);
    return berhasil;
}

void AuthManager::logout() {
    sesiSaatIni = UserAktif{};
    UI::cetakPeringatan("Anda telah logout dari sistem.");
}

void AuthManager::tampilkanDaftarUser() const {
    const char* sql = "SELECT id, username, role, nama_lengkap FROM users ORDER BY id ASC;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db.getHandle(), sql, -1, &stmt, nullptr) != SQLITE_OK) return;

    std::cout << "\n" << UI::CYAN << "==========================================================\n";
    std::cout << std::left << std::setw(6) << "ID"
              << std::setw(15) << "USERNAME"
              << std::setw(12) << "ROLE"
              << std::setw(25) << "NAMA LENGKAP" << "\n";
    std::cout << "==========================================================" << UI::RESET << "\n";

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        std::cout << std::left << std::setw(6) << sqlite3_column_int(stmt, 0)
                  << std::setw(15) << reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1))
                  << std::setw(12) << reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2))
                  << std::setw(25) << reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3)) << "\n";
    }
    std::cout << "==========================================================\n";
    sqlite3_finalize(stmt);
}

bool AuthManager::tambahUserBaru(const std::string& username, const std::string& password, const std::string& role, const std::string& namaLengkap) {
    if (role != "admin" && role != "kasir") {
        UI::cetakError("Role harus 'admin' atau 'kasir'!");
        return false;
    }

    std::string passHash = hashPassword(password);
    const char* sql = "INSERT INTO users (username, password_hash, role, nama_lengkap) VALUES (?, ?, ?, ?);";

    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db.getHandle(), sql, -1, &stmt, nullptr) != SQLITE_OK) return false;

    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, passHash.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, role.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 4, namaLengkap.c_str(), -1, SQLITE_TRANSIENT);

    bool sukses = (sqlite3_step(stmt) == SQLITE_DONE);
    sqlite3_finalize(stmt);
    return sukses;
}

bool AuthManager::hapusUser(const std::string& username) {
    if (username == sesiSaatIni.username) {
        UI::cetakError("Tidak bisa menghapus akun yang sedang login!");
        return false;
    }
    const char* sql = "DELETE FROM users WHERE username = ?;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db.getHandle(), sql, -1, &stmt, nullptr) != SQLITE_OK) return false;

    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_step(stmt);
    int berubah = sqlite3_changes(db.getHandle());
    sqlite3_finalize(stmt);
    return berubah > 0;
}