# ✨ Chat Server Project ✨

Selamat datang di dokumentasi *Tugas Besar Sistem Operasi*! ✨ Proyek ini bertujuan untuk mengembangkan **Chat Server** sederhana namun fungsional, yang menampilkan komunikasi *real-time* antar klien melalui protokol tertentu. Berikut adalah penjelasan detail dari proyek ini:

---

## ✨ Fitur Utama
- 🔗 **Koneksi Multi-klien**: Mendukung banyak klien terhubung secara bersamaan.
- 📢 **Broadcast Pesan**: Pesan dapat disiarkan ke semua klien yang terhubung.
- ⚡ **Respons Cepat**: Komunikasi *real-time* dengan waktu latensi rendah.
- 🔐 **Keamanan Dasar**: Implementasi autentikasi pengguna sederhana.
- 🚀 **Ekstensibilitas**: Mudah untuk dikembangkan lebih lanjut.

---

## ⚙ Teknologi yang Digunakan
- **Bahasa Pemrograman**: C 💚
- **Protokol**: Socket TCP/IP 🔹
- **Arsitektur**: Client-Server ⚖
- **Implementasi Proses**: Menggunakan `fork` (tanpa `thread`).

---

## 🗃 Struktur Proyek
```
chat-server-project/
├── chat_server_tubes.c   # Implementasi server
├── chat_client_tubes.c   # Implementasi klien
├── README.md             # Dokumentasi proyek
└── chat_log.txt      # Dependensi 
```

---

## 🔎 Cara Kerja
### 1. Server
- Server akan mendengarkan koneksi masuk dari klien pada port tertentu.
- Setiap pesan dari klien akan diterima dan disiarkan ke semua klien yang terhubung.

### 2. Klien
- Klien dapat terhubung ke server melalui IP dan port.
- Pesan yang dikirim oleh klien akan diteruskan oleh server ke semua klien lain.

---

## 🚀 Cara Menjalankan
### Persiapan
1. Pastikan kompiler C sudah terinstal. 🔧
2. Kompilasi file `chat_server_tubes.c` dan `chat_client_tubes.c`:
   ```bash
   gcc -o chat_server chat_server_tubes.c
   gcc -o chat_client chat_client_tubes.c
   ```

### Jalankan Server
1. Buka terminal dan navigasikan ke direktori proyek.
2. Jalankan perintah:
   ```bash
   ./chat_server
   ```

### Jalankan Klien
1. Buka terminal baru.
2. Jalankan perintah:
   ```bash
   ./chat_client
   ```

3. Masukkan IP dan port server saat diminta.
4. Mulai berkirim pesan! 🙌

---

## 🔧 Pengembangan Lebih Lanjut
Beberapa ide pengembangan tambahan:
- ✨ **UI/UX**: Tambahkan antarmuka pengguna berbasis GUI dengan library seperti GTK atau Qt.
- ⚡ **Enkripsi**: Implementasikan enkripsi pesan untuk meningkatkan keamanan.
- 🌐 **Support Web**: Gunakan protokol HTTP untuk menjadikan server kompatibel dengan aplikasi berbasis web.
- ⌚ **Penyimpanan Pesan**: Log pesan di file atau database untuk keperluan *audit trail*.

---

## 💡 Catatan Penting
- Pastikan koneksi jaringan stabil untuk pengalaman terbaik. 🚀
- Jangan gunakan aplikasi ini untuk keperluan produksi tanpa implementasi keamanan yang memadai. ⚠

---

## 🎨 Kontributor
- 👨‍💻 **Fitri Salwa** (NIM: 231524009)  
- 👩‍💻 **Salma Nesya Putri Salia** (NIM: 231524024)  

---

## 📢 Lisensi
Proyek ini dilisensikan di bawah lisensi **MIT**. Silakan gunakan, modifikasi, dan distribusikan dengan bebas. 🙏

---

🎉 Terima kasih telah membaca! Selamat coding! ✨

