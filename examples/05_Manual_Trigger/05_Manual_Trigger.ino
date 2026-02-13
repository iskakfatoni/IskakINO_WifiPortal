#include <IskakINO_WifiPortal.h>

IskakINO_WifiPortal portal;

// Tentukan pin tombol (misal GPIO 0 atau D3 di NodeMCU)
const int TRIGGER_PIN = 0; 

void setup() {
    Serial.begin(115200);
    
    // Set up pin tombol sebagai input
    pinMode(TRIGGER_PIN, INPUT_PULLUP);

    portal.setBrandName("IskakINO Trigger");

    // Jalankan portal seperti biasa
    // Jika sudah ada config, dia akan langsung konek ke WiFi
    portal.begin("IskakINO-Manual", "12345678");
}

void loop() {
    // Rutinitas wajib library
    portal.tick();

    // LOGIKA MANUAL TRIGGER:
    // Jika tombol ditekan (LOW) selama loop berjalan
    if (digitalRead(TRIGGER_PIN) == LOW) {
        Serial.println("[IskakINO] Tombol ditekan! Membuka portal secara paksa...");
        
        // Memanggil kembali setupPortal() untuk mengaktifkan AP & DNS
        portal.setupPortal();
        
        // Beri sedikit delay agar tidak memicu berkali-kali
        delay(1000);
    }

    // Kode utama kamu di sini...
    static unsigned long lastMsg = 0;
    if (millis() - lastMsg > 5000) {
        if (WiFi.status() == WL_CONNECTED) {
            Serial.println("Aplikasi berjalan normal... WiFi OK!");
        } else {
            Serial.println("Aplikasi berjalan dalam mode Portal.");
        }
        lastMsg = millis();
    }
}
