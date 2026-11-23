# Aplikasi Kasir CLI (C++17)

Aplikasi *Point of Sale* (POS) berbasis terminal (*Command Line Interface*) yang dibangun menggunakan bahasa C++ modern (C++17). Repository ini disusun secara bertahap per versi untuk mendokumentasikan evolusi arsitektur kode dan pengembangan fitur dari masa ke masa.

---

## 🎯 Tujuan Pembuatan Project

Project ini dikembangkan secara berkelanjutan sebagai bagian dari portofolio *software engineering* untuk mendemonstrasikan penerapan praktik pemrograman C++ yang bersih, terstruktur, dan aman:

1. **Modularitas Kode:** Memisahkan deklarasi data dan kontrak fungsi (`.h`) dari implementasi logika bisnis (`.cpp`) serta alur utama program (`main.cpp`).
2. **Manajemen Memori & Kontainer Dinamis:** Menggunakan kontainer STL (`std::vector`), *pass-by-reference* (`&`), dan *const-reference* (`const &`) untuk efisiensi memori.
3. **Defensive Programming:** Mencegah *crash* maupun *infinite loop* akibat kesalahan input pengguna pada `std::cin` melalui validasi *stream* dan pembersihan *buffer*.
4. **Operasi File & Sistem:** Mengimplementasikan penyimpanan data transaksi permanen ke file eksternal (`<fstream>`) serta pencatatan waktu real-time (`<ctime>`).

---

## ✨ Fitur Utama

* **Manajemen Keranjang Belanja:** Menambah, melihat, mengubah jumlah, dan menghapus daftar barang secara dinamis.
* **Kalkulasi Transaksi Otomatis:** Menghitung subtotal, menerapkan logika diskon, serta menghitung kembalian pembayaran tunai.
* **Pencetakan Struk Eksternal:** Mengekspor rincian bukti transaksi lengkap dengan stempel waktu ke dalam file `.txt`.

---

## 📂 Struktur Repository

Setiap tahapan pengembangan disimpan dalam direktori terpisah (`Cashier_V1`, `Cashier_V2`, dst.) dengan struktur standar berikut:

```text
Cashier_Vx/
├── CMakeLists.txt     # Konfigurasi build system lintas platform
├── include/           # File header (.h) untuk deklarasi struct dan fungsi
└── src/               # File source code (.cpp) untuk logika dan alur utama
