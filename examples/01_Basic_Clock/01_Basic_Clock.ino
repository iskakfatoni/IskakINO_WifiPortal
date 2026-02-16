/*
 * Library: IskakINO_FastNTP
 * Example: 01_Basic_Clock
 * ------------------------------------------------------------
 * Deskripsi:
 * Contoh paling dasar penggunaan IskakINO_FastNTP yang digabung 
 * dengan IskakINO_WifiPortal. Menampilkan waktu ke Serial Monitor.
 * ------------------------------------------------------------
 */

#if defined(ESP8266)
  #include <ESP8266WiFi.h>
#elif defined(ESP32)
  #include <WiFi.h>
#endif

#include <WiFiUdp.h>
#include <IskakINO_WifiPortal.h>  // Portal WiFi
#include <IskakINO_FastNTP.h>     // Library NTP Fast Non-Blocking

// Inisialisasi UDP dan NTP
WiFiUDP ntpUDP;
// Gunakan server NTP Indonesia untuk latensi lebih rendah
IskakINO_FastNTP ntp(ntpUDP, "id.pool.ntp.org"); 

// Inisialisasi Portal
IskakINO_WifiPortal portal;

unsigned long prevMs = 0;

void setup() {
  Serial.begin(115200);
  
  // 1. Mulai Portal (Jika WiFi belum diset, akan buat AP "IskakINO_Setup")
  portal.begin("IskakINO_Setup");

  // 2. Mulai NTP (Offset 25200 detik = GMT+7 / WIB)
  ntp.begin(25200); 
  
  // Mengatur interval update ke server setiap 1 jam (3600000 ms)
  ntp.setSyncInterval(3600000);

  Serial.println(F("System Initialized..."));
}

void loop() {
  // WAJIB: Selalu jalankan handle portal agar koneksi terjaga
  portal.handle();

  // WAJIB: Jalankan state machine NTP (Non-blocking)
  // Fungsi ini otomatis mengecek apakah WiFi terhubung sebelum kirim paket
  if (WiFi.status() == WL_CONNECTED) {
    ntp.update();
  }

  // Tampilkan waktu setiap 1 detik
  if (millis() - prevMs >= 1000) {
    prevMs = millis();

    if (ntp.isTimeSet()) {
      // Contoh output: Senin, 16-02-2026 13:00:01
      String clockNow = ntp.getDayName(LANG_ID);
      clockNow += ", ";
      clockNow += ntp.getFormattedDate('-');
      clockNow += " ";
      clockNow += ntp.getFormattedTime();
      
      Serial.println(clockNow);
    } else {
      Serial.println(F("Waiting for NTP sync..."));
    }
  }
}
