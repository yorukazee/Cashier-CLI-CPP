#ifndef AUTH_MANAGER_H
#define AUTH_MANAGER_H

#include "Database.h"
#include <string>

struct UserAktif {
    int id = -1;
    std::string username;
    std::string role;
    std::string namaLengkap;
    bool sudahLogin = false;
};

class AuthManager {
private:
    Database& db;
    UserAktif sesiSaatIni;

    std::string hashPassword(const std::string& password) const;

public:
    explicit AuthManager(Database& database);

    bool login();
    void logout();
    const UserAktif& getUserAktif() const { return sesiSaatIni; }

    void tampilkanDaftarUser() const;
    bool tambahUserBaru(const std::string& username, const std::string& password, const std::string& role, const std::string& namaLengkap);
    bool hapusUser(const std::string& username);
};

#endif