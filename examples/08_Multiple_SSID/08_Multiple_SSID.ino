#include <IskakINO_WifiPortal.h>

IskakINO_WifiPortal portal;

// Struktur data untuk daftar WiFi
struct WiFiConfig {
  const char* ssid;
  const char* pass;
};

// Daftar WiFi cadangan (Bisa ditambah sesuai kebutuhan)
WiFiConfig listWiFi[] = {
  {"WiFi_Utama", "password123"},
  {"Hotspot_HP", "bayarindulu"},
  {"WiFi_Kantor", "kantor2024"}
};

const int totalWiFi = sizeof(listWiFi) / sizeof(listWiFi[0]);

void setup() {
    Serial.begin(115200);
    portal.setBrandName("IskakINO MultiWiFi");

    Serial.println("\n[IskakINO] Mencoba daftar WiFi yang tersimpan...");

    bool connected = false;

    // Looping mencoba setiap WiFi di daftar
    for (int i = 0; i < totalWiFi; i++) {
        Serial.printf("\n[IskakINO] Mencoba: %s\n", listWiFi[i].ssid);
        
        // Gunakan parameter langsung di begin()
        // Kita beri timeout singkat per WiFi (misal 10 detik)
        WiFi.begin(listWiFi[i].ssid, listWiFi[i].pass);
        
        int retry = 0;
        while (WiFi.status() != WL_CONNECTED && retry < 20) { // 20 * 500ms = 10 detik
            delay(500);
            Serial.print(".");
            retry++;
        }

        if (WiFi.status() == WL_CONNECTED) {
            Serial.println("\n[IskakINO] Terhubung ke: " + String(listWiFi[i].ssid));
            connected = true;
            break; // Berhenti looping kalau sudah konek
        }
    }

    // Jika semua daftar gagal, baru buka portal
    if (!connected) {
        Serial.println("\n[IskakINO] Semua WiFi gagal. Membuka Portal...");
        portal.begin("IskakINO-Config", "12345678");
    }
}

void loop() {
    portal.tick();
    
    // Aplikasi utama kamu
    if (WiFi.status() == WL_CONNECTED) {
        // Lakukan pengiriman data...
    }
}
