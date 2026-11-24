#ifndef DATABASE_H
#define DATABASE_H

#include "sqlite3.h"
#include <string>

class Database {
private:
    sqlite3* db;
    std::string dbPath;

    void inisialisasiTabel();
    void isiDataAwalJikaKosong();

public:
    explicit Database(const std::string& path);
    ~Database();

    // Mencegah duplikasi objek koneksi database (RAII)
    Database(const Database&) = delete;
    Database& operator=(const Database&) = delete;

    sqlite3* getHandle() const { return db; }
    bool eksekusiSQL(const std::string& sql) const;
};

#endif