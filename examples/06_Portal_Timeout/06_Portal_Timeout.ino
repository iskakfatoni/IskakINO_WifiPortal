#include <IskakINO_WifiPortal.h>

IskakINO_WifiPortal portal;

// Batas waktu portal aktif (misal 2 menit = 120.000 ms)
const unsigned long PORTAL_TIMEOUT = 120000; 

void setup() {
    Serial.begin(115200);

    portal.setBrandName("IskakINO Timeout");

    // Mencoba koneksi, jika gagal portal terbuka
    if (!portal.begin("IskakINO-Timer", "12345678")) {
        Serial.println("[IskakINO] Portal aktif. Waktu kamu 2 menit!");
    }
}

void loop() {
    portal.tick();

    // Cek jika portal sedang aktif
    // Kita bisa mengecek status WiFi atau variabel portal jika public
    if (WiFi.getMode() == WIFI_AP || WiFi.getMode() == WIFI_AP_STA) {
        
        // Hitung berapa lama portal sudah berjalan
        // Catatan: _portalStartTime harus public atau ada getter-nya
        if (millis() > PORTAL_TIMEOUT) {
            Serial.println("[IskakINO] Waktu habis! Restarting untuk mencoba koneksi ulang...");
            delay(1000);
            ESP.restart();
        }
    }

    // Aplikasi kamu tetap berjalan...
}
