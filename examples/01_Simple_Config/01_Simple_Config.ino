/*
 * IskakINO_WifiPortal - SimpleConfig Example
 * * Cara Penggunaan:
 * 1. Upload sketch ini ke ESP32 atau ESP8266.
 * 2. Alat akan mencoba konek ke WiFi terakhir.
 * 3. Jika gagal, cari WiFi "IskakINO_AP" di HP Anda.
 * 4. Portal akan muncul: Masukkan WiFi, API Key, dan ID Sensor.
 */

#include <IskakINO_WifiPortal.h>

IskakINO_WifiPortal portal;

// Siapkan variabel untuk menampung input dari Portal
char myApiKey[32] = "default_key";
char sensorId[10] = "SN-001";

void setup() {
  Serial.begin(115200);
  delay(1000);

  // 1. Tambahkan parameter kustom (muncul di halaman web)
  // Format: (ID, Label, Variabel, Panjang_Karakter)
  portal.addParameter("api_key", "API Key Cloud", myApiKey, 32);
  portal.addParameter("dev_id", "Sensor ID", sensorId, 10);

  // 2. Aktifkan Fitur Web OTA (Update firmware via browser)
  portal.enableOTA(true);

  // 3. Atur nama Brand di halaman web
  portal.setBrandName("IskakINO Smart Home");

  // 4. Mulai Portal (Nama AP, Password AP)
  // Jika gagal konek WiFi lama, otomatis buka Portal selama 5 menit
  portal.setPortalTimeout(300); 
  
  if (portal.begin("IskakINO_AP", "12345678")) {
    Serial.println("Koneksi Berhasil!");
    Serial.print("API Key yang digunakan: ");
    Serial.println(myApiKey);
  } else {
    Serial.println("Mode Portal Aktif...");
  }
}

void loop() {
  // Wajib dipanggil agar Web Server dan DNS tetap berjalan
  portal.tick();

  // Jalankan logika IoT Anda hanya jika sudah terkoneksi
  if (WiFi.status() == WL_CONNECTED) {
    static unsigned long lastMsg = 0;
    if (millis() - lastMsg > 10000) {
      Serial.println("Mengirim data dengan Key: " + String(myApiKey));
      lastMsg = millis();
    }
  }
}
