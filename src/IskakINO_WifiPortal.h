#ifndef ISKAKINO_WIFIPORTAL_H
#define ISKAKINO_WIFIPORTAL_H

#include <Arduino.h>
#include <DNSServer.h>

// --- Seleksi Arsitektur Otomatis ---
#if defined(ESP32)
  #include <WiFi.h>
  #include <WebServer.h>
  #include <Preferences.h>
  #include <Update.h>
  using IskakWebServer = WebServer;
#elif defined(ESP8266)
  #include <ESP8266WiFi.h>
  #include <ESP8266WebServer.h>
  #include <LittleFS.h>
  #include <Updater.h>
  using IskakWebServer = ESP8266WebServer;
#endif

// Struktur untuk Parameter Kustom
struct IskakParam {
  const char* id;
  const char* label;
  char* value;
  int length;
};

class IskakINO_WifiPortal {
  public:
    IskakINO_WifiPortal();
    
    // --- Fungsi Utama ---
    // Mencoba konek ke WiFi tersimpan, jika gagal buka Portal
    bool begin(const char* apName, const char* apPass = NULL);
    void tick(); 
    void resetSettings();
    void setBrandName(const char* name);

    // --- Custom Parameters ---
    void addParameter(const char* id, const char* label, char* value, int length);

    // --- Maintenance ---
    void enableOTA(bool status);
    void setPortalTimeout(int sec); 

  private:
    IskakWebServer* _server;
    DNSServer _dnsServer;
    
    const char* _apName;
    const char* _apPass;
    const char* _brandName = "IskakINO Portal";
    
    bool _portalActive = false;
    bool _otaEnabled = false;
    int  _timeout = 0;
    unsigned long _portalStartTime;

    IskakParam* _params[10];
    uint8_t _paramCount = 0;

    // --- Handlers ---
    void setupPortal();
    void handleRoot();      
    void handleSave();      
    void handleOTA();       
    void handleDoOTA();     
    
    // --- Storage & Helpers ---
    bool loadConfig();
    void saveConfig(String ssid, String pass);
    String buildHTML();      
    String getSystemInfo();  
};

#endif
