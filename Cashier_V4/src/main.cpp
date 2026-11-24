#include "../include/Database.h"
#include "../include/AuthManager.h"
#include "../include/Inventaris.h"
#include "../include/MesinKasir.h"

int main() {
    Database db("data/kasir.db");
    AuthManager auth(db);
    Inventaris gudang(db);
    MesinKasir kasir(db, auth, gudang);

    kasir.jalankan();

    return 0;
}