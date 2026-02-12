/*
  IskakINO_WifiPortal - 02 Basic Portal
  -------------------------------------
  Contoh dasar penggunaan portal WiFi dengan fitur:
  - Auto WiFi Scan (Memilih SSID tanpa mengetik)
  - System Dashboard (Cek IP, RAM, & Uptime)
  - Maintenance (Tombol Restart & Reset via Web)
*/

#include <IskakINO_WifiPortal.h>

IskakINO_WifiPortal portal;

void setup() {
  // Gunakan baudrate 115200 agar pembacaan Serial lancar
  Serial.begin(115200);
  delay(1000);

  Serial.println("\n[IskakINO] Memulai Contoh 02_Basic_Portal");

  /**
   * portal.begin(Nama_AP, Password_AP)
   * Jika tidak diberi password (seperti contoh di bawah), 
   * maka WiFi AP akan terbuka tanpa sandi.
   */
  if (portal.begin("IskakINO_Config_AP")) {
    Serial.println("\n[OK] Alat terhubung ke WiFi!");
    Serial.print("[OK] IP Address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\n[!] Gagal konek/Data kosong.");
    Serial.println("[!] Portal Aktif: Konek ke 'IskakINO_Config_AP'");
  }
}

void loop() {
  /**
   * WAJIB: portal.tick() harus ada di dalam loop.
   * Fungsi ini menangani:
   * 1. Web Server (Halaman Config)
   * 2. DNS Server (Captive Portal agar otomatis pop-up di HP)
   * 3. Auto-Reconnect jika WiFi terputus
   */
  portal.tick();

  // Tambahkan kode aplikasi kamu di bawah sini
  // Misalnya: Membaca sensor setiap 10 detik
  static unsigned long lastCheck = 0;
  if (millis() - lastCheck > 10000) {
    if (WiFi.status() == WL_CONNECTED) {
      Serial.println("Sistem Online...");
    } else {
      Serial.println("Sistem Offline (Portal Aktif)...");
    }
    lastCheck = millis();
  }
}
