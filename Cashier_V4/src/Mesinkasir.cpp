#include "../include/MesinKasir.h"
#include "../include/UIHelper.h"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <ctime>

MesinKasir::MesinKasir(Database& database, AuthManager& authManager, Inventaris& inv)
    : db(database), auth(authManager), inventaris(inv) {}

void MesinKasir::jalankan() {
    while (true) {
        std::cout << "\n" << UI::BIRU << UI::BOLD << "==========================================\n";
        std::cout << "   POINT OF SALE (POS) ENTERPRISE v4.0    \n";
        std::cout << "==========================================" << UI::RESET << "\n";
        std::cout << "1. Login Masuk Sistem\n";
        std::cout << "0. Keluar Aplikasi\n";

        int pilih = UI::bacaInt("Pilih menu: ", 0, 1);
        if (pilih == 0) {
            UI::cetakSukses("Aplikasi ditutup. Terima kasih!");
            break;
        }

        if (auth.login()) {
            keranjang.clear();
            if (auth.getUserAktif().role == "admin") {
                menuAdmin();
            } else {
                menuKasir();
            }
        }
    }
}

void MesinKasir::menuKasir() {
    int pilihan = -1;
    do {
        const auto& user = auth.getUserAktif();
        std::cout << "\n" << UI::CYAN << "=== MODE KASIR | Petugas: " << user.namaLengkap << " ===" << UI::RESET << "\n";
        inventaris.cekStokMenipis(5);
        std::cout << "\n1. Lihat Katalog Barang\n";
        std::cout << "2. Cari Barang\n";
        std::cout << "3. Tambah Barang ke Keranjang\n";
        std::cout << "4. Lihat Keranjang\n";
        std::cout << "5. Edit Jumlah di Keranjang\n";
        std::cout << "6. Hapus Barang dari Keranjang\n";
        std::cout << "7. Proses Pembayaran & Cetak Struk\n";
        std::cout << "0. Logout\n";

        pilihan = UI::bacaInt("Pilih aksi: ", 0, 7);
        switch (pilihan) {
            case 1: inventaris.tampilkanKatalog(); break;
            case 2: {
                std::string key = UI::bacaBaris("Masukkan nama/kode barang: ");
                inventaris.cariProduk(key);
                break;
            }
            case 3: tambahKeKeranjang(); break;
            case 4: tampilkanKeranjang(); break;
            case 5: editJumlahKeranjang(); break;
            case 6: hapusDariKeranjang(); break;
            case 7: prosesPembayaran(); break;
            case 0: auth.logout(); break;
        }
    } while (pilihan != 0);
}

void MesinKasir::menuAdmin() {
    int pilihan = -1;
    do {
        const auto& user = auth.getUserAktif();
        std::cout << "\n" << UI::KUNING << UI::BOLD << "=== MODE ADMIN | Petugas: " << user.namaLengkap << " ===" << UI::RESET << "\n";
        inventaris.cekStokMenipis(5);
        std::cout << "\n1. Lihat Katalog & Stok\n";
        std::cout << "2. Tambah Produk Baru\n";
        std::cout << "3. Restock Barang (Tambah Stok)\n";
        std::cout << "4. Ubah Harga Barang\n";
        std::cout << "5. Hapus Produk\n";
        std::cout << "6. Kelola Kode Promo & Pajak (PPN)\n";
        std::cout << "7. Kelola Akun Kasir / Admin\n";
        std::cout << "8. Laporan Penjualan & Produk Terlaris\n";
        std::cout << "9. Masuk ke Mode Kasir (Transaksi)\n";
        std::cout << "0. Logout\n";

        pilihan = UI::bacaInt("Pilih aksi: ", 0, 9);
        switch (pilihan) {
            case 1: inventaris.tampilkanKatalog(); break;
            case 2: {
                Produk p;
                p.kode = UI::bacaBaris("Kode Barang (misal BRG06): ");
                p.nama = UI::bacaBaris("Nama Barang: ");
                p.harga = UI::bacaDouble("Harga Satuan: Rp ", 0);
                p.stok = UI::bacaInt("Stok Awal: ", 0);

                if (inventaris.tambahProdukBaru(p)) {
                    UI::cetakSukses("Produk baru berhasil disimpan ke database!");
                } else {
                    UI::cetakError("Gagal menyimpan! Kode barang mungkin sudah digunakan.");
                }
                break;
            }
            case 3: {
                inventaris.tampilkanKatalog();
                std::string kode = UI::bacaBaris("Kode Barang: ");
                int tambah = UI::bacaInt("Jumlah stok masuk: ", 1);
                if (inventaris.tambahStokProduk(kode, tambah)) {
                    UI::cetakSukses("Stok berhasil ditambahkan!");
                } else {
                    UI::cetakError("Kode barang tidak ditemukan.");
                }
                break;
            }
            case 4: {
                inventaris.tampilkanKatalog();
                std::string kode = UI::bacaBaris("Kode Barang: ");
                double harga = UI::bacaDouble("Harga Baru: Rp ", 0);
                if (inventaris.ubahHargaProduk(kode, harga)) {
                    UI::cetakSukses("Harga barang berhasil diperbarui!");
                } else {
                    UI::cetakError("Kode barang tidak ditemukan.");
                }
                break;
            }
            case 5: {
                inventaris.tampilkanKatalog();
                std::string kode = UI::bacaBaris("Kode Barang yang dihapus: ");
                if (inventaris.hapusProduk(kode)) {
                    UI::cetakSukses("Produk berhasil dihapus!");
                } else {
                    UI::cetakError("Kode barang tidak ditemukan.");
                }
                break;
            }
            case 6: menuKelolaPromoDanPajak(); break;
            case 7: menuKelolaUser(); break;
            case 8: menuLaporanAnalitik(); break;
            case 9: menuKasir(); return;
            case 0: auth.logout(); break;
        }
    } while (pilihan != 0);
}

void MesinKasir::menuKelolaPromoDanPajak() {
    int pilih = -1;
    do {
        std::cout << "\n--- KELOLA PROMO & PPN ---\n";
        std::cout << "Status PPN Saat Ini : " << (isPpnAktif() ? UI::HIJAU + "AKTIF (" + std::to_string((int)getTarifPpn()) + "%)" : UI::MERAH + "NONAKTIF") << UI::RESET << "\n";
        std::cout << "1. Lihat Daftar Kode Promo\n";
        std::cout << "2. Tambah Kode Promo Baru\n";
        std::cout << "3. Hapus Kode Promo\n";
        std::cout << "4. Aktifkan / Nonaktifkan PPN\n";
        std::cout << "5. Ubah Persentase Tarif PPN\n";
        std::cout << "0. Kembali\n";

        pilih = UI::bacaInt("Pilih aksi: ", 0, 5);
        switch (pilih) {
            case 1: {
                const char* sql = "SELECT kode_promo, persen_diskon, aktif FROM promo;";
                sqlite3_stmt* stmt;
                if (sqlite3_prepare_v2(db.getHandle(), sql, -1, &stmt, nullptr) == SQLITE_OK) {
                    std::cout << "\n--- DAFTAR KODE PROMO ---\n";
                    while (sqlite3_step(stmt) == SQLITE_ROW) {
                        std::cout << "- Kode: " << UI::BOLD << reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)) << UI::RESET
                                  << " | Diskon: " << sqlite3_column_double(stmt, 1) << "%\n";
                    }
                    sqlite3_finalize(stmt);
                }
                break;
            }
            case 2: {
                std::string kode = UI::bacaBaris("Kode Promo (misal DISKON15): ");
                double persen = UI::bacaDouble("Persen Diskon (1 - 100): ", 1);
                if (persen > 100) persen = 100;

                const char* sql = "INSERT OR REPLACE INTO promo (kode_promo, persen_diskon, aktif) VALUES (?, ?, 1);";
                sqlite3_stmt* stmt;
                if (sqlite3_prepare_v2(db.getHandle(), sql, -1, &stmt, nullptr) == SQLITE_OK) {
                    sqlite3_bind_text(stmt, 1, kode.c_str(), -1, SQLITE_TRANSIENT);
                    sqlite3_bind_double(stmt, 2, persen);
                    sqlite3_step(stmt);
                    sqlite3_finalize(stmt);
                    UI::cetakSukses("Kode promo '" + kode + "' berhasil disimpan!");
                }
                break;
            }
            case 3: {
                std::string kode = UI::bacaBaris("Kode Promo yang dihapus: ");
                const char* sql = "DELETE FROM promo WHERE kode_promo = ?;";
                sqlite3_stmt* stmt;
                if (sqlite3_prepare_v2(db.getHandle(), sql, -1, &stmt, nullptr) == SQLITE_OK) {
                    sqlite3_bind_text(stmt, 1, kode.c_str(), -1, SQLITE_TRANSIENT);
                    sqlite3_step(stmt);
                    int berubah = sqlite3_changes(db.getHandle());
                    sqlite3_finalize(stmt);
                    if (berubah > 0) UI::cetakSukses("Promo berhasil dihapus!");
                    else UI::cetakError("Kode promo tidak ditemukan.");
                }
                break;
            }
            case 4: {
                std::string statusBaru = isPpnAktif() ? "0" : "1";
                db.eksekusiSQL("UPDATE pengaturan SET nilai = '" + statusBaru + "' WHERE kunci = 'ppn_aktif';");
                UI::cetakSukses("Status PPN berhasil diubah!");
                break;
            }
            case 5: {
                double tarifBaru = UI::bacaDouble("Masukkan tarif PPN baru (%): ", 0);
                db.eksekusiSQL("UPDATE pengaturan SET nilai = '" + std::to_string((int)tarifBaru) + "' WHERE kunci = 'tarif_ppn';");
                UI::cetakSukses("Tarif PPN berhasil diperbarui!");
                break;
            }
        }
    } while (pilih != 0);
}

void MesinKasir::menuKelolaUser() {
    int pilih = -1;
    do {
        std::cout << "\n--- KELOLA AKUN PENGGUNA ---\n";
        std::cout << "1. Lihat Daftar User\n";
        std::cout << "2. Tambah User Baru (Kasir / Admin)\n";
        std::cout << "3. Hapus User\n";
        std::cout << "0. Kembali\n";

        pilih = UI::bacaInt("Pilih aksi: ", 0, 3);
        switch (pilih) {
            case 1: auth.tampilkanDaftarUser(); break;
            case 2: {
                std::string user = UI::bacaBaris("Username baru: ");
                std::string pass = UI::bacaBaris("Password: ");
                std::string role = UI::bacaBaris("Role (admin / kasir): ");
                std::string nama = UI::bacaBaris("Nama Lengkap: ");

                if (auth.tambahUserBaru(user, pass, role, nama)) {
                    UI::cetakSukses("Akun '" + user + "' berhasil dibuat!");
                } else {
                    UI::cetakError("Gagal membuat akun (username mungkin sudah dipakai).");
                }
                break;
            }
            case 3: {
                auth.tampilkanDaftarUser();
                std::string user = UI::bacaBaris("Username yang akan dihapus: ");
                if (auth.hapusUser(user)) {
                    UI::cetakSukses("User berhasil dihapus!");
                } else {
                    UI::cetakError("Gagal menghapus user.");
                }
                break;
            }
        }
    } while (pilih != 0);
}

void MesinKasir::menuLaporanAnalitik() const {
    int pilih = -1;
    do {
        std::cout << "\n--- LAPORAN PENJUALAN & ANALITIK ---\n";
        std::cout << "1. Laporan Semua Transaksi\n";
        std::cout << "2. Filter Laporan Berdasarkan Tanggal (YYYY-MM-DD)\n";
        std::cout << "3. Filter Laporan Berdasarkan Kasir\n";
        std::cout << "4. Laporan Produk Terlaris (Best Seller)\n";
        std::cout << "0. Kembali\n";

        pilih = UI::bacaInt("Pilih laporan: ", 0, 4);
        switch (pilih) {
            case 1: laporanSemuaTransaksi(); break;
            case 2: {
                std::string tgl = UI::bacaBaris("Masukkan tanggal (contoh 2026-11-24): ");
                laporanBerdasarkanTanggal(tgl);
                break;
            }
            case 3: {
                std::string kasir = UI::bacaBaris("Masukkan username kasir (contoh kasir1): ");
                laporanBerdasarkanKasir(kasir);
                break;
            }
            case 4: laporanProdukTerlaris(); break;
        }
    } while (pilih != 0);
}

void MesinKasir::tambahKeKeranjang() {
    inventaris.tampilkanKatalog();
    std::string kode = UI::bacaBaris("Masukkan Kode Barang: ");

    auto produkOpt = inventaris.cariBerdasarkanKode(kode);
    if (!produkOpt.has_value()) {
        UI::cetakError("Kode barang '" + kode + "' tidak ditemukan!");
        return;
    }

    const Produk& produk = produkOpt.value();
    if (produk.stok <= 0) {
        UI::cetakError("Stok '" + produk.nama + "' sedang kosong!");
        return;
    }

    int sudahDiKeranjang = 0;
    for (const auto& item : keranjang) {
        if (item.kode == kode) {
            sudahDiKeranjang = item.jumlah;
            break;
        }
    }

    int sisaTersedia = produk.stok - sudahDiKeranjang;
    if (sisaTersedia <= 0) {
        UI::cetakPeringatan("Seluruh stok yang tersedia sudah berada di keranjang!");
        return;
    }

    int qty = UI::bacaInt("Jumlah beli (Tersedia " + std::to_string(sisaTersedia) + "): ", 1, sisaTersedia);

    for (auto& item : keranjang) {
        if (item.kode == kode) {
            item.jumlah += qty;
            UI::cetakSukses("Jumlah '" + item.nama + "' diperbarui menjadi " + std::to_string(item.jumlah) + " pcs!");
            return;
        }
    }

    keranjang.push_back({produk.kode, produk.nama, produk.harga, qty});
    UI::cetakSukses("'" + produk.nama + "' (" + std::to_string(qty) + " pcs) ditambahkan ke keranjang!");
}

void MesinKasir::tampilkanKeranjang() const {
    if (keranjang.empty()) {
        UI::cetakPeringatan("Keranjang belanja masih kosong.");
        return;
    }

    double subtotal = 0;
    std::cout << "\n--- KERANJANG BELANJA ---\n";
    std::cout << std::fixed << std::setprecision(0);
    for (size_t i = 0; i < keranjang.size(); ++i) {
        double sub = keranjang[i].harga * keranjang[i].jumlah;
        subtotal += sub;
        std::cout << i + 1 << ". [" << keranjang[i].kode << "] " << keranjang[i].nama
                  << " (" << keranjang[i].jumlah << " x Rp " << keranjang[i].harga << ") = Rp " << sub << "\n";
    }
    std::cout << "-------------------------\n";
    std::cout << "Subtotal Sementara: Rp " << subtotal << "\n";
}

void MesinKasir::editJumlahKeranjang() {
    if (keranjang.empty()) {
        UI::cetakPeringatan("Keranjang kosong.");
        return;
    }
    tampilkanKeranjang();

    int nomor = UI::bacaInt("Pilih nomor item yang diubah (0 batal): ", 0, static_cast<int>(keranjang.size()));
    if (nomor == 0) return;

    ItemKeranjang& item = keranjang[nomor - 1];
    auto produkOpt = inventaris.cariBerdasarkanKode(item.kode);
    int maksStok = produkOpt.has_value() ? produkOpt->stok : item.jumlah;

    int qtyBaru = UI::bacaInt("Jumlah baru untuk '" + item.nama + "' (1 - " + std::to_string(maksStok) + "): ", 1, maksStok);
    item.jumlah = qtyBaru;
    UI::cetakSukses("Jumlah berhasil diubah!");
}

void MesinKasir::hapusDariKeranjang() {
    if (keranjang.empty()) {
        UI::cetakPeringatan("Keranjang kosong.");
        return;
    }
    tampilkanKeranjang();

    int nomor = UI::bacaInt("Pilih nomor item yang dihapus (0 batal): ", 0, static_cast<int>(keranjang.size()));
    if (nomor == 0) return;

    std::string namaDihapus = keranjang[nomor - 1].nama;
    keranjang.erase(keranjang.begin() + (nomor - 1));
    UI::cetakSukses("'" + namaDihapus + "' dihapus dari keranjang.");
}

double MesinKasir::cekPersenPromo(const std::string& kodePromo) const {
    if (kodePromo.empty()) return 0.0;
    const char* sql = "SELECT persen_diskon FROM promo WHERE kode_promo = ? AND aktif = 1;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db.getHandle(), sql, -1, &stmt, nullptr) != SQLITE_OK) return 0.0;

    sqlite3_bind_text(stmt, 1, kodePromo.c_str(), -1, SQLITE_TRANSIENT);
    double persen = 0.0;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        persen = sqlite3_column_double(stmt, 0);
    }
    sqlite3_finalize(stmt);
    return persen;
}

bool MesinKasir::isPpnAktif() const {
    const char* sql = "SELECT nilai FROM pengaturan WHERE kunci = 'ppn_aktif';";
    sqlite3_stmt* stmt;
    bool aktif = true;
    if (sqlite3_prepare_v2(db.getHandle(), sql, -1, &stmt, nullptr) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            std::string val = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
            aktif = (val == "1");
        }
        sqlite3_finalize(stmt);
    }
    return aktif;
}

double MesinKasir::getTarifPpn() const {
    const char* sql = "SELECT nilai FROM pengaturan WHERE kunci = 'tarif_ppn';";
    sqlite3_stmt* stmt;
    double tarif = 11.0;
    if (sqlite3_prepare_v2(db.getHandle(), sql, -1, &stmt, nullptr) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            std::string val = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
            tarif = std::stod(val);
        }
        sqlite3_finalize(stmt);
    }
    return tarif;
}

int MesinKasir::simpanTransaksiKeDB(double subtotal, double diskon, double ppn, double totalAkhir) const {
    std::time_t now = std::time(nullptr);
    char tglStr[20], jamStr[20];
    std::strftime(tglStr, sizeof(tglStr), "%Y-%m-%d", std::localtime(&now));
    std::strftime(jamStr, sizeof(jamStr), "%H:%M:%S", std::localtime(&now));

    const auto& user = auth.getUserAktif();
    const char* sqlTrans = "INSERT INTO transaksi (tanggal, waktu, kasir_username, kasir_nama, subtotal, diskon_promo, pajak_ppn, total_akhir) "
                           "VALUES (?, ?, ?, ?, ?, ?, ?, ?);";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db.getHandle(), sqlTrans, -1, &stmt, nullptr) != SQLITE_OK) return -1;

    sqlite3_bind_text(stmt, 1, tglStr, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, jamStr, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, user.username.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 4, user.namaLengkap.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_double(stmt, 5, subtotal);
    sqlite3_bind_double(stmt, 6, diskon);
    sqlite3_bind_double(stmt, 7, ppn);
    sqlite3_bind_double(stmt, 8, totalAkhir);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        sqlite3_finalize(stmt);
        return -1;
    }
    sqlite3_finalize(stmt);

    int idTransaksi = static_cast<int>(sqlite3_last_insert_rowid(db.getHandle()));

    // Simpan rincian barang ke detail_transaksi
    const char* sqlDetail = "INSERT INTO detail_transaksi (transaksi_id, kode_produk, nama_produk, harga_satuan, jumlah) "
                            "VALUES (?, ?, ?, ?, ?);";
    sqlite3_stmt* stmtDetail;
    if (sqlite3_prepare_v2(db.getHandle(), sqlDetail, -1, &stmtDetail, nullptr) == SQLITE_OK) {
        for (const auto& item : keranjang) {
            sqlite3_bind_int(stmtDetail, 1, idTransaksi);
            sqlite3_bind_text(stmtDetail, 2, item.kode.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(stmtDetail, 3, item.nama.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_double(stmtDetail, 4, item.harga);
            sqlite3_bind_int(stmtDetail, 5, item.jumlah);
            sqlite3_step(stmtDetail);
            sqlite3_reset(stmtDetail);
        }
        sqlite3_finalize(stmtDetail);
    }

    return idTransaksi;
}

void MesinKasir::cetakStrukKeFile(int idTransaksi, const std::string& kodePromo, double subtotal, double diskon, double ppn, double totalAkhir, double tunai, double kembalian) const {
    std::time_t now = std::time(nullptr);
    char namaFile[100], waktuStr[100];
    std::strftime(namaFile, sizeof(namaFile), "data/struk_%Y%m%d_%H%M%S.txt", std::localtime(&now));
    std::strftime(waktuStr, sizeof(waktuStr), "%d-%m-%Y %H:%M:%S", std::localtime(&now));

    std::ofstream file(namaFile);
    if (!file.is_open()) return;

    file << std::fixed << std::setprecision(0);
    file << "========================================\n";
    file << "            STRUK PEMBAYARAN            \n";
    file << "No. Transaksi : #" << idTransaksi << "\n";
    file << "Waktu         : " << waktuStr << "\n";
    file << "Kasir         : " << auth.getUserAktif().namaLengkap << "\n";
    file << "========================================\n";
    for (size_t i = 0; i < keranjang.size(); ++i) {
        file << i + 1 << ". " << keranjang[i].nama << "\n"
             << "   " << keranjang[i].jumlah << " x Rp " << keranjang[i].harga
             << " = Rp " << (keranjang[i].harga * keranjang[i].jumlah) << "\n";
    }
    file << "----------------------------------------\n";
    file << "Subtotal      : Rp " << subtotal << "\n";
    if (diskon > 0) {
        file << "Promo (" << kodePromo << ") : -Rp " << diskon << "\n";
    }
    if (ppn > 0) {
        file << "PPN (" << (int)getTarifPpn() << "%)     : +Rp " << ppn << "\n";
    }
    file << "Total Akhir   : Rp " << totalAkhir << "\n";
    file << "Tunai         : Rp " << tunai << "\n";
    file << "Kembalian     : Rp " << kembalian << "\n";
    file << "========================================\n";
    file.close();

    UI::cetakSukses("Struk transaksi disimpan ke '" + std::string(namaFile) + "'");
}

void MesinKasir::prosesPembayaran() {
    if (keranjang.empty()) {
        UI::cetakPeringatan("Keranjang kosong, tidak ada transaksi.");
        return;
    }

    tampilkanKeranjang();

    double subtotal = 0;
    for (const auto& item : keranjang) {
        subtotal += item.harga * item.jumlah;
    }

    std::string kodePromo = UI::bacaBaris("Masukkan Kode Promo (tekan Enter jika tidak ada): ");
    double persenDiskon = cekPersenPromo(kodePromo);
    double diskon = 0;

    if (!kodePromo.empty()) {
        if (persenDiskon > 0) {
            diskon = subtotal * (persenDiskon / 100.0);
            UI::cetakSukses("Kode promo '" + kodePromo + "' diterapkan! Diskon " + std::to_string((int)persenDiskon) + "% (-Rp " + std::to_string((int)diskon) + ")");
        } else {
            UI::cetakError("Kode promo tidak valid atau tidak aktif.");
        }
    }

    double setelahDiskon = subtotal - diskon;
    double ppn = isPpnAktif() ? (setelahDiskon * (getTarifPpn() / 100.0)) : 0.0;
    double totalAkhir = setelahDiskon + ppn;

    std::cout << std::fixed << std::setprecision(0);
    std::cout << "----------------------------------------\n";
    std::cout << "Subtotal      : Rp " << subtotal << "\n";
    if (diskon > 0) std::cout << "Diskon Promo  : -Rp " << diskon << "\n";
    if (ppn > 0) std::cout << "PPN (" << (int)getTarifPpn() << "%)     : +Rp " << ppn << "\n";
    std::cout << UI::BOLD << "TOTAL BAYAR   : Rp " << totalAkhir << UI::RESET << "\n";
    std::cout << "----------------------------------------\n";

    double tunai = UI::bacaDouble("Uang Tunai    : Rp ", totalAkhir);
    double kembalian = tunai - totalAkhir;
    std::cout << UI::HIJAU << "Kembalian     : Rp " << kembalian << UI::RESET << "\n";

    inventaris.kurangiStokSetelahTransaksi(keranjang);
    int idTransaksi = simpanTransaksiKeDB(subtotal, diskon, ppn, totalAkhir);
    cetakStrukKeFile(idTransaksi, kodePromo, subtotal, diskon, ppn, totalAkhir, tunai, kembalian);

    keranjang.clear();
    UI::cetakSukses("Transaksi #" + std::to_string(idTransaksi) + " selesai & database diperbarui!");
}

void MesinKasir::laporanSemuaTransaksi() const {
    const char* sql = "SELECT id, tanggal, waktu, kasir_nama, total_akhir FROM transaksi ORDER BY id DESC;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db.getHandle(), sql, -1, &stmt, nullptr) != SQLITE_OK) return;

    double totalOmzet = 0;
    int jumlah = 0;

    std::cout << "\n" << UI::CYAN << "=== LAPORAN SELURUH TRANSAKSI ===" << UI::RESET << "\n";
    std::cout << std::fixed << std::setprecision(0);

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        jumlah++;
        double total = sqlite3_column_double(stmt, 4);
        totalOmzet += total;
        std::cout << "#" << sqlite3_column_int(stmt, 0)
                  << " | " << reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1))
                  << " " << reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2))
                  << " | Kasir: " << reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3))
                  << " | Total: Rp " << total << "\n";
    }
    sqlite3_finalize(stmt);

    std::cout << "-----------------------------------------\n";
    std::cout << "Total Transaksi : " << jumlah << "\n";
    std::cout << UI::HIJAU << UI::BOLD << "Total Omzet     : Rp " << totalOmzet << UI::RESET << "\n";
}

void MesinKasir::laporanBerdasarkanTanggal(const std::string& tanggal) const {
    const char* sql = "SELECT id, waktu, kasir_nama, total_akhir FROM transaksi WHERE tanggal = ? ORDER BY id ASC;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db.getHandle(), sql, -1, &stmt, nullptr) != SQLITE_OK) return;

    sqlite3_bind_text(stmt, 1, tanggal.c_str(), -1, SQLITE_TRANSIENT);

    double totalOmzet = 0;
    int jumlah = 0;
    std::cout << "\n=== LAPORAN TANGGAL " << tanggal << " ===\n";
    std::cout << std::fixed << std::setprecision(0);

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        jumlah++;
        double total = sqlite3_column_double(stmt, 3);
        totalOmzet += total;
        std::cout << "#" << sqlite3_column_int(stmt, 0)
                  << " [" << reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)) << "]"
                  << " | Kasir: " << reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2))
                  << " | Rp " << total << "\n";
    }
    sqlite3_finalize(stmt);

    std::cout << "Total Transaksi: " << jumlah << " | Omzet: Rp " << totalOmzet << "\n";
}

void MesinKasir::laporanBerdasarkanKasir(const std::string& usernameKasir) const {
    const char* sql = "SELECT id, tanggal, waktu, total_akhir FROM transaksi WHERE kasir_username = ? ORDER BY id DESC;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db.getHandle(), sql, -1, &stmt, nullptr) != SQLITE_OK) return;

    sqlite3_bind_text(stmt, 1, usernameKasir.c_str(), -1, SQLITE_TRANSIENT);

    double totalOmzet = 0;
    int jumlah = 0;
    std::cout << "\n=== LAPORAN KINERJA KASIR: " << usernameKasir << " ===\n";
    std::cout << std::fixed << std::setprecision(0);

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        jumlah++;
        double total = sqlite3_column_double(stmt, 3);
        totalOmzet += total;
        std::cout << "#" << sqlite3_column_int(stmt, 0)
                  << " | " << reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1))
                  << " " << reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2))
                  << " | Rp " << total << "\n";
    }
    sqlite3_finalize(stmt);

    std::cout << "Total Transaksi Ditangani: " << jumlah << " | Total Omzet: Rp " << totalOmzet << "\n";
}

void MesinKasir::laporanProdukTerlaris() const {
    const char* sql = "SELECT kode_produk, nama_produk, SUM(jumlah) AS total_terjual, SUM(harga_satuan * jumlah) AS total_pendapatan "
                      "FROM detail_transaksi "
                      "GROUP BY kode_produk, nama_produk "
                      "ORDER BY total_terjual DESC;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db.getHandle(), sql, -1, &stmt, nullptr) != SQLITE_OK) return;

    std::cout << "\n" << UI::KUNING << UI::BOLD << "=== PERINGKAT PRODUK TERLARIS (BEST SELLER) ===" << UI::RESET << "\n";
    std::cout << std::fixed << std::setprecision(0);

    int peringkat = 1;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        std::cout << peringkat++ << ". [" << reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)) << "] "
                  << reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1))
                  << " -> Terjual: " << UI::HIJAU << sqlite3_column_int(stmt, 2) << " pcs" << UI::RESET
                  << " (Omzet: Rp " << sqlite3_column_double(stmt, 3) << ")\n";
    }

    if (peringkat == 1) {
        UI::cetakPeringatan("Belum ada data penjualan barang.");
    }
    sqlite3_finalize(stmt);
}