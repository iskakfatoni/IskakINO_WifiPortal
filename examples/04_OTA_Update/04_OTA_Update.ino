/*
  IskakINO_WifiPortal - 04 OTA Update
  -----------------------------------
  Contoh cara mengaktifkan fitur pembaruan firmware nirkabel (OTA).
  Anda bisa mengirim file .bin hasil compile langsung melalui browser.
*/

#include <IskakINO_WifiPortal.h>

IskakINO_WifiPortal portal;

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("\n[IskakINO] Memulai Contoh 04_OTA_Update");

  // 1. AKTIFKAN FITUR OTA
  // Panggil fungsi ini SEBELUM portal.begin().
  // Ini akan membuka jalur khusus di alamat: http://192.168.4.1/update
  portal.enableOTA(true);

  // 2. MULAI PORTAL
  if (portal.begin("IskakINO_OTA_AP")) {
    Serial.println("\n[OK] Terhubung! Akses OTA via IP Lokal peranti.");
  } else {
    Serial.println("\n[INFO] Portal Aktif.");
    Serial.println("Akses http://192.168.4.1/update untuk update firmware.");
  }
}

void loop() {
  // 3. JALANKAN PROSES
  // portal.tick() akan menangani proses penerimaan file firmware 
  // dan proses penulisan ke memori flash secara otomatis.
  portal.tick();

  // Kode aplikasi kamu tetap berjalan di sini
}
