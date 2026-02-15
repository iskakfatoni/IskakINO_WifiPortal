# 🚀 IskakINO_WifiPortal

**IskakINO_WifiPortal** adalah library manajemen WiFi yang *powerful*, ringan, dan estetik untuk **ESP8266** dan **ESP32**. Library ini dirancang untuk memudahkan pengembang IoT dalam membuat perangkat yang bisa dikonfigurasi lewat web browser tanpa perlu melakukan *hardcode* SSID dan Password pada kode program.

---

## ✨ Fitur Utama
* 📱 **Captive Portal Otomatis**: Portal konfigurasi akan terbuka otomatis jika WiFi tidak ditemukan atau gagal terhubung.
* 📊 **Dashboard Sistem**: Pantau kesehatan perangkat (Free RAM, Uptime, dan IP Address) secara real-time langsung dari browser.
* 🔍 **WiFi Scanner**: Pengguna cukup memilih SSID dari daftar jaringan yang tersedia (tidak perlu mengetik manual).
* 💾 **Custom Parameters**: Simpan data tambahan seperti *API Token*, *User ID*, atau *Topic MQTT* ke memori permanen (LittleFS/Preferences).
* ☁️ **OTA (Over-The-Air) Update**: Perbarui firmware perangkat secara nirkabel melalui halaman web portal.
* 🎨 **Modern UI**: Antarmuka web yang responsif dengan *Dark Mode* yang nyaman di mata.
* 🖱️ **Manual Trigger**: Mendukung penggunaan tombol fisik untuk memaksa portal terbuka kembali kapan saja.

---

## 📂 Contoh Penggunaan (Examples)
Library ini menyertakan 10 contoh lengkap untuk berbagai skenario project IoT:

1.  **01_Simple_Config**: Dasar penggunaan koneksi WiFi otomatis.
2.  **02_Basic_Portal**: Cara kustomisasi nama AP dan password portal.
3.  **03_Custom_Parameters**: Implementasi penyimpanan variabel kustom.
4.  **04_OTA_Update**: Panduan fitur pembaruan firmware jarak jauh.
5.  **05_Manual_Trigger**: Cara memicu portal menggunakan tombol fisik (GPIO).
6.  **06_Portal_Timeout**: Menutup portal otomatis untuk keamanan & penghematan daya.
7.  **07_Deep_Sleep**: Implementasi pada perangkat berbasis baterai.
8.  **08_Multiple_SSID**: Logika prioritas koneksi ke beberapa jaringan WiFi.
9.  **09_Web_Terminal**: Mengirim perintah dan menerima log Serial via Web.
10. **10_NTP_Clock**: Sinkronisasi waktu otomatis (WIB) tanpa modul RTC.

---

## 🛠️ Instalasi

1.  Unduh repositori ini dalam format **.ZIP**.
2.  Buka Arduino IDE, pilih menu **Sketch** -> **Include Library** -> **Add .ZIP Library...**.
3.  Pilih file yang sudah diunduh tadi.
4.  Pastikan board ESP32 atau ESP8266 sudah terpasang di Board Manager kamu.

---

## 🚀 Cara Penggunaan Singkat

Berikut adalah cara paling simpel untuk mengimplementasikan library ini:

```cpp
#include <IskakINO_WifiPortal.h>

IskakINO_WifiPortal portal;

void setup() {
    Serial.begin(115200);
    
    // Konfigurasi Brand dan Mulai Portal
    portal.setBrandName("IskakINO Project");
    portal.begin("IskakINO-AP", "12345678");
}

void loop() {
    // Wajib dipanggil agar server tetap berjalan
    portal.tick();
}
```

## 📋 Persyaratan Sistem
Library ini dibangun menggunakan library standar berikut (tidak perlu instal library tambahan):
ESP8266: ESP8266WiFi, ESP8266WebServer, DNSServer, LittleFS.
ESP32: WiFi, WebServer, DNSServer, Update, Preferences.

## 🤝 Kontribusi
Ingin menambahkan fitur atau melaporkan bug? Silakan buat Pull Request atau ajukan di kolom Issues. Kontribusi kamu sangat berarti untuk pengembangan library ini!

## 📂 Struktur Library
* **src/** : Berisi kode sumber utama (`IskakINO_WifiPortal.h` & `IskakINO_WifiPortal.cpp`).
* **examples/** : Contoh penggunaan library untuk memudahkan pengguna baru.
* **library.properties** : Metadata resmi untuk Arduino Library Manager.
* **keywords.txt** : Definisi pewarnaan sintaks (syntax highlighting) di Arduino IDE.

## 📝 Lisensi
Proyek ini dilisensikan di bawah **MIT License**. Artinya, siapa pun bebas menggunakan, mengubah, dan mendistribusikan kode ini selama tetap mencantumkan kredit kepada penulis aslinya (**Iskak**).
