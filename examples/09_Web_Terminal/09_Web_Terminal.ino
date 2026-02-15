#include <IskakINO_WifiPortal.h>

IskakINO_WifiPortal portal;

// Variabel untuk menampung log dan perintah
char cmdLog[100] = "Waiting for command...";
char lastResponse[50] = "None";

void setup() {
    Serial.begin(115200);

    // Tambahkan parameter sebagai "Input Terminal"
    portal.addParameter("cmd", "Kirim Perintah (LED_ON/LED_OFF)", cmdLog, 100);
    
    portal.setBrandName("IskakINO Terminal");
    portal.begin("IskakINO-Terminal", "12345678");

    pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
    portal.tick();

    // Logika Terminal: Cek apakah isi cmdLog berubah?
    String currentCmd = String(cmdLog);
    currentCmd.trim(); // Bersihkan spasi

    if (currentCmd == "LED_ON") {
        digitalWrite(LED_BUILTIN, LOW); // On (ESP8266 Active Low)
        Serial.println("[Terminal] LED dinyalakan!");
        strncpy(cmdLog, "Status: LED IS ON", 100); 
    } 
    else if (currentCmd == "LED_OFF") {
        digitalWrite(LED_BUILTIN, HIGH); // Off
        Serial.println("[Terminal] LED dimatikan!");
        strncpy(cmdLog, "Status: LED IS OFF", 100);
    }
    else if (currentCmd == "RESTART") {
        ESP.restart();
    }

    // Aplikasi utama kamu...
}
