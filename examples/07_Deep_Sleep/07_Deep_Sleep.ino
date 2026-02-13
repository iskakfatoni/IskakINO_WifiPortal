#include <IskakINO_WifiPortal.h>

IskakINO_WifiPortal portal;

// Pin untuk memicu portal (karena di Deep Sleep, loop tidak berjalan)
const int TRIGGER_PIN = 0; 

void setup() {
    Serial.begin(115200);
    pinMode(TRIGGER_PIN, INPUT_PULLUP);

    Serial.println("\n[IskakINO] Bangun dari tidur...");

    // 1. Cek apakah user menahan tombol saat booting?
    // Jika iya, buka portal konfigurasi.
    if (digitalRead(TRIGGER_PIN) == LOW) {
        Serial.println("[IskakINO] Tombol ditekan, masuk mode Portal...");
        portal.begin("IskakINO-Config", "12345678");
        // Dalam mode portal, kita tidak boleh tidur dulu
    } else {
        // 2. Jika tidak ditekan, coba konek WiFi secara cepat
        if (portal.begin("IskakINO-Config", "12345678")) {
            Serial.println("[IskakINO] Sukses Konek! Kirim data...");
            
            // SIMULASI KIRIM DATA
            delay(1000); 
            Serial.println("[IskakINO] Data terkirim. Tidur lagi dalam 5 detik.");
            delay(5000);
            
            // MASUK DEEP SLEEP (Tidur selama 15 menit)
            // ESP8266: GPIO16 harus nyambung ke RST
            // ESP32: Tidak butuh jumper tambahan
            Serial.println("Zzz...");
            ESP.deepSleep(15 * 60 * 1000000); 
        } else {
            // 3. Jika gagal konek dan tidak ada tombol, tetap buka portal 
            // agar user tahu ada yang salah dengan WiFi-nya
            Serial.println("[IskakINO] WiFi Gagal. Portal terbuka otomatis.");
        }
    }
}

void loop() {
    // Tick tetap jalan jika portal aktif
    portal.tick();
    
    // Jika portal aktif, kita bisa tambahkan timeout agar tidak nyala terus
    if (millis() > 300000) { // 5 menit
        Serial.println("[IskakINO] Portal timeout. Tidur...");
        ESP.deepSleep(0); // Tidur selamanya sampai di-reset manual
    }
}
