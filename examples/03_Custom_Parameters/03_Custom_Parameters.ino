/*
  IskakINO_WifiPortal - 03 Custom Parameters
  ------------------------------------------
  Contoh cara menambahkan input tambahan di halaman portal.
  Data yang diinput user akan tersimpan permanen di memori Flash (LittleFS).
*/

#include <IskakINO_WifiPortal.h>

IskakINO_WifiPortal portal;

// 1. SIAPKAN VARIABEL PENAMPUNG
// Buat array karakter (char) untuk menampung data dari portal.
// Pastikan panjang array cukup untuk menampung teks yang diinput user.
char myApiKey[40] = "api_default_123"; 
char mqttServer[30] = "broker.hivemq.com";
int  updateInterval = 60; // Contoh parameter angka

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("\n[IskakINO] Memulai Contoh 03_Custom_Parameters");

  // 2. DAFTARKAN PARAMETER KE PORTAL
  // addParameter(ID_KUNCI, LABEL_DI_WEB, VARIABEL, PANJANG_MAKS)
  // ID_KUNCI harus unik dan digunakan untuk identifikasi data di file config.json
  
  portal.addParameter("api_key", "Masukkan API Key", myApiKey, 40);
  portal.addParameter("mqtt_host", "Broker MQTT", mqttServer, 30);

  // 3. MULAI PORTAL
  // Saat begin() dijalankan, library akan otomatis mencoba membaca file 
  // config.json. Jika ada data lama, variabel di atas akan terisi otomatis.
  if (portal.begin("IskakINO_Param_AP")) {
    Serial.println("\n[OK] Terhubung ke WiFi!");
    
    // Menampilkan data yang berhasil dimuat (hasil input user sebelumnya)
    Serial.println("--- Data Kustom Terload ---");
    Serial.print("API Key    : "); Serial.println(myApiKey);
    Serial.print("MQTT Server: "); Serial.println(mqttServer);
    Serial.println("---------------------------");
  } else {
    Serial.println("\n[INFO] WiFi tidak tersedia. Portal Aktif.");
    Serial.println("Silakan buka 192.168.4.1 untuk isi API Key.");
  }
}

void loop() {
  // 4. JALANKAN PROSES LATAR BELAKANG
  // tick() akan menangani penyimpanan data otomatis saat user klik 'SAVE' di web.
  portal.tick();

  // 5. CONTOH PENGGUNAAN DATA
  // Di sini kamu bisa menggunakan variabel 'myApiKey' untuk keperluan programmu.
  static unsigned long lastMsg = 0;
  if (millis() - lastMsg > 10000 && WiFi.status() == WL_CONNECTED) {
    Serial.print("Mengirim data menggunakan API Key: ");
    Serial.println(myApiKey);
    lastMsg = millis();
  }
}
