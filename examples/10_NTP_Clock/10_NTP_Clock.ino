#include <IskakINO_WifiPortal.h>
#include <time.h> // Library waktu standar C++

IskakINO_WifiPortal portal;

// Konfigurasi Waktu Indonesia (WIB = GMT+7)
const long gmtOffset_sec = 7 * 3600; 
const int daylightOffset_sec = 0;
const char* ntpServer = "pool.ntp.org";

void printLocalTime() {
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo)) {
        Serial.println("[NTP] Gagal mengambil waktu");
        return;
    }
    // Format: Hari, Tanggal Bulan Tahun Jam:Menit:Detik
    Serial.println(&timeinfo, "[WAKTU] %A, %d %B %Y %H:%M:%S");
}

void setup() {
    Serial.begin(115200);
    portal.setBrandName("IskakINO Clock");

    // Mulai portal
    if (portal.begin("IskakINO-Clock", "12345678")) {
        Serial.println("[IskakINO] WiFi OK! Sinkronisasi jam...");
        
        // Inisialisasi NTP
        configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
    }
}

void loop() {
    portal.tick();

    // Tampilkan jam setiap 5 detik jika sudah terhubung WiFi
    static unsigned long lastUpdate = 0;
    if (WiFi.status() == WL_CONNECTED && millis() - lastUpdate > 5000) {
        printLocalTime();
        lastUpdate = millis();
    }
}
