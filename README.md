# IskakINO_WifiPortal 🚀
**IskakINO_WifiPortal** adalah library Arduino yang ringan dan "zero-dependency" untuk mengelola koneksi WiFi pada ESP32 dan ESP8266. Dilengkapi dengan Captive Portal modern bertema gelap, fitur OTA (Over-The-Air) update, dan Dashboard sistem.

![Compile Test](https://github.com/iskakfatoni/IskakINO_WifiPortal/actions/workflows/compile_test.yml/badge.svg)

## ✨ Fitur Utama
- **Captive Portal Otomatis:** Otomatis mengarahkan browser ke halaman konfigurasi saat terhubung ke Access Point.
- **Custom Parameters:** Tambahkan input kustom (seperti API Key, MQTT Broker, dll) langsung dari halaman web.
- **Web-Based OTA:** Update firmware perangkat secara nirkabel melalui browser.
- **System Dashboard:** Pantau status RAM, tipe Chip, dan Uptime perangkat secara real-time.
- **Hybrid Support:** Satu kode yang sama untuk ESP32 dan ESP8266 (Agnostik Hardware).
- **Modern Dark UI:** Tampilan antarmuka yang bersih dan responsif di perangkat mobile.

## 🆕 Fitur Terbaru
- **Auto WiFi Scan:** Tidak perlu mengetik SSID secara manual. Portal akan memindai jaringan di sekitar dan Anda cukup klik untuk mengisi otomatis.
- **Enhanced Dashboard:** Info real-time alamat IP, Uptime perangkat, dan penggunaan Free RAM yang lebih akurat.
- **Maintenance Actions:**
  - **Restart:** Reboot perangkat secara remote melalui dashboard.
  - **Reset Info:** Menghapus seluruh konfigurasi WiFi dan Parameter yang tersimpan di LittleFS untuk kembali ke pengaturan pabrik.
- **Smart Auto-Reconnect:** Logika cerdas untuk mencoba menyambung kembali ke WiFi jika koneksi terputus tanpa menghentikan program utama.

---

## 🖥️ Tampilan Dashboard & Maintenance
Setelah terhubung ke Access Point perangkat (Default IP: `192.168.4.1`), Anda akan melihat panel kontrol baru:



### Cara Kerja Tombol:
1. **Daftar WiFi:** Klik pada nama SSID yang muncul, maka kolom input SSID di bawahnya akan terisi otomatis.
2. **Restart:** Memicu `ESP.restart()` melalui perintah HTTP.
3. **Reset Info:** Melakukan format pada `LittleFS` untuk membersihkan semua data sensitif atau konfigurasi lama yang salah.

---

## 🛠 Instalasi
1. Download repository ini sebagai `.zip`.
2. Buka Arduino IDE, pilih menu **Sketch** > **Include Library** > **Add .ZIP Library...**.
3. Pilih file yang baru saja didownload.
4. Jangan lupa instal dependensi (untuk ESP8266 pastikan LittleFS sudah terpasang).

## 🚀 Cara Penggunaan Singkat

```cpp
#include <IskakINO_WifiPortal.h>

IskakINO_WifiPortal portal;
char myApiKey[32] = "default_key";

void setup() {
  portal.addParameter("api_key", "API Key", myApiKey, 32);
  portal.enableOTA(true);
  
  if (portal.begin("IskakINO_AP")) {
    // Terhubung ke WiFi!
  }
}

void loop() {
  portal.tick(); // Wajib untuk menjalankan web server & DNS
}
```

## 📂 Struktur Library
* **src/** : Berisi kode sumber utama (`IskakINO_WifiPortal.h` & `IskakINO_WifiPortal.cpp`).
* **examples/** : Contoh penggunaan library untuk memudahkan pengguna baru.
* **library.properties** : Metadata resmi untuk Arduino Library Manager.
* **keywords.txt** : Definisi pewarnaan sintaks (syntax highlighting) di Arduino IDE.

## 📝 Lisensi
Proyek ini dilisensikan di bawah **MIT License**. Artinya, siapa pun bebas menggunakan, mengubah, dan mendistribusikan kode ini selama tetap mencantumkan kredit kepada penulis aslinya (**Iskak**).
