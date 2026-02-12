#include "IskakINO_WifiPortal.h"

IskakINO_WifiPortal::IskakINO_WifiPortal() {
    _server = new IskakWebServer(80);
    _paramCount = 0;
}

bool IskakINO_WifiPortal::begin(const char* apName, const char* apPass) {
    _apName = apName;
    _apPass = apPass;
    _portalStartTime = millis();

    Serial.println("\n[IskakINO] Memulai Portal...");

    if (loadConfig()) {
        WiFi.mode(WIFI_STA);
        WiFi.begin(); 
        
        Serial.print("[IskakINO] Menghubungkan");
        int timeout = 0;
        while (WiFi.status() != WL_CONNECTED && timeout < 40) {
            delay(500);
            Serial.print(".");
            timeout++;
        }
    }

    if (WiFi.status() != WL_CONNECTED) {
        setupPortal();
        return false;
    }

    Serial.println("\n[IskakINO] Terhubung! IP: " + WiFi.localIP().toString());
    return true;
}

void IskakINO_WifiPortal::addParameter(const char* id, const char* label, char* value, int length) {
    if (_paramCount < 10) {
        _params[_paramCount] = new IskakParam{id, label, value, length};
        _paramCount++;
    }
}

void IskakINO_WifiPortal::setupPortal() {
    WiFi.mode(WIFI_AP);
    WiFi.softAP(_apName, _apPass);
    _dnsServer.start(53, "*", WiFi.softAPIP());

    _server->on("/", std::bind(&IskakINO_WifiPortal::handleRoot, this));
    _server->on("/save", HTTP_POST, std::bind(&IskakINO_WifiPortal::handleSave, this));
    
    if (_otaEnabled) {
        _server->on("/update", HTTP_GET, std::bind(&IskakINO_WifiPortal::handleOTA, this));
        _server->on("/update", HTTP_POST, [&]() {
            _server->sendHeader("Connection", "close");
            _server->send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
            ESP.restart();
        }, [&]() {
            HTTPUpload& upload = _server->upload();
            if (upload.status == UPLOAD_FILE_START) {
                #if defined(ESP32)
                    if (!Update.begin(UPDATE_SIZE_UNKNOWN)) Update.printError(Serial);
                #elif defined(ESP8266)
                    // Di ESP8266, kita harus menentukan ukuran atau menggunakan trik U_FS/U_FLASH
                    uint32_t maxSketchSpace = (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000;
                    if (!Update.begin(maxSketchSpace)) Update.printError(Serial);
                #endif
            } else if (upload.status == UPLOAD_FILE_WRITE) {
                if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) Update.printError(Serial);
            } else if (upload.status == UPLOAD_FILE_END) {
                if (Update.end(true)) Serial.println("OTA Success");
            }
        });
    }

    _server->onNotFound(std::bind(&IskakINO_WifiPortal::handleRoot, this));
    _server->begin();
    _portalActive = true;
    Serial.println("[IskakINO] Portal Aktif di 192.168.4.1");
}

String IskakINO_WifiPortal::buildHTML() {
    String s = "<html><head><meta name='viewport' content='width=device-width,initial-scale=1'><style>";
    s += "body{font-family:sans-serif;background:#121212;color:#eee;padding:20px;text-align:center;}";
    s += "input{width:100%;padding:12px;margin:8px 0;border-radius:5px;border:1px solid #333;background:#222;color:#fff;}";
    s += "button{width:100%;padding:12px;background:#00adb5;border:none;color:#fff;font-weight:bold;margin-top:10px;cursor:pointer;}";
    s += ".info{background:#1e1e1e;padding:15px;border-radius:10px;margin-bottom:20px;text-align:left;font-size:0.9em;}";
    s += "</style></head><body>";
    s += "<h1>" + String(_brandName) + "</h1>";
    s += "<div class='info'>" + getSystemInfo() + "</div>";
    s += "<form action='/save' method='POST'>";
    s += "<h3>WiFi Config</h3>";
    s += "<input name='s' placeholder='SSID' required>";
    s += "<input name='p' type='password' placeholder='Password'>";
    
    if (_paramCount > 0) {
        s += "<h3>Parameters</h3>";
        for (int i = 0; i < _paramCount; i++) {
            s += "<input name='" + String(_params[i]->id) + "' placeholder='" + String(_params[i]->label) + "'>";
        }
    }
    
    s += "<button type='submit'>SAVE & RECONNECT</button></form>";
    if (_otaEnabled) s += "<br><a href='/update' style='color:#555;font-size:0.8em;'>Firmware Update</a>";
    s += "</body></html>";
    return s;
}

String IskakINO_WifiPortal::getSystemInfo() {
    String info = "<b>System Dashboard</b><br>";
    #if defined(ESP32)
        info += "Chip: " + String(ESP.getChipModel()) + "<br>";
    #elif defined(ESP8266)
        info += "Chip: ESP8266<br>";
        info += "Core: " + String(ESP.getCoreVersion()) + "<br>";
    #endif
    info += "Free RAM: " + String(ESP.getFreeHeap() / 1024) + " KB<br>";
    info += "Free RAM: " + String(ESP.getFreeHeap() / 1024) + " KB<br>";
    info += "Uptime: " + String(millis() / 1000) + " sec";
    return info;
}

void IskakINO_WifiPortal::handleSave() {
    String ssid = _server->arg("s");
    String pass = _server->arg("p");
    
    // Simpan Custom Params jika ada
    for (int i = 0; i < _paramCount; i++) {
        String val = _server->arg(_params[i]->id);
        strncpy(_params[i]->value, val.c_str(), _params[i]->length);
    }
    
    saveConfig(ssid, pass);
    _server->send(200, "text/html", "Saved! Restarting...");
    delay(2000);
    ESP.restart();
}

void IskakINO_WifiPortal::tick() {
    if (_portalActive) {
        _dnsServer.processNextRequest();
    }
    _server.handleClient();

    // Fitur Auto-Reconnect setiap 30 detik jika tidak dalam mode Portal
    if (!_portalActive && millis() - _lastWifiCheck > 30000) {
        if (WiFi.status() != WL_CONNECTED) {
            Serial.println("[IskakINO] WiFi disconnected! Reconnecting...");
            WiFi.reconnect();
            _reconnectAttempts++;
            
            // Jika gagal 5 kali berturut-turut, buka portal lagi
            if (_reconnectAttempts >= _maxReconnectAttempts) {
                Serial.println("[IskakINO] Failed to reconnect. Opening Portal...");
                startPortal(); 
            }
        } else {
            _reconnectAttempts = 0; // Reset jika berhasil konek
        }
        _lastWifiCheck = millis();
    }
}

// Implementasi Storage & Misc Functions...
void IskakINO_WifiPortal::setBrandName(const char* name) { _brandName = name; }
void IskakINO_WifiPortal::enableOTA(bool status) { _otaEnabled = status; }
void IskakINO_WifiPortal::setPortalTimeout(int sec) { _timeout = sec; }

void IskakINO_WifiPortal::handleRoot() {
    // 1. SCANNING WIFI: Mencari jaringan di sekitar sebelum halaman dimuat
    // WiFi.scanNetworks() mengembalikan jumlah jaringan yang ditemukan
    int n = WiFi.scanNetworks();
    String wifiList = "";
    
    if (n == 0) {
        wifiList = "<p style='color:#888;'>No networks found. Refresh to try again.</p>";
    } else {
        // Loop untuk menyusun daftar WiFi ke dalam elemen HTML
        for (int i = 0; i < n; ++i) {
            String ssid = WiFi.SSID(i);
            // Setiap baris WiFi diberi event 'onclick' untuk memicu fungsi JavaScript fillSSID
            wifiList += "<div class='nw' onclick=\"fillSSID('" + ssid + "')\">";
            wifiList += "<span>" + ssid + "</span>";
            wifiList += "<span class='rssi'>" + String(WiFi.RSSI(i)) + " dBm</span>"; // Menampilkan kekuatan sinyal
            wifiList += "</div>";
        }
    }

    // 2. HTML & CSS: Membangun antarmuka web portal
    String html = "<html><head><title>IskakINO Config</title>";
    html += "<meta name='viewport' content='width=device-width, initial-scale=1'>"; // Agar responsif di HP
    
    // CSS: Mengatur tampilan portal (Dark Theme)
    html += "<style>";
    html += "body{background:#1a1a1a;color:#eee;font-family:sans-serif;text-align:center;padding:20px;}";
    html += ".card{background:#252525;padding:20px;border-radius:10px;box-shadow:0 4px 10px rgba(0,0,0,0.5);max-width:400px;margin:0 auto;}";
    html += "h2{color:#00d1b2;} h3{font-size:1.1em;color:#888;text-align:left;margin-bottom:10px;}";
    
    // Styling untuk kotak daftar WiFi yang bisa di-scroll
    html += ".net-box{margin-bottom:20px;max-height:180px;overflow-y:auto;background:#111;border-radius:5px;border:1px solid #444;}";
    html += ".nw{display:flex;justify-content:space-between;padding:12px;border-bottom:1px solid #333;cursor:pointer;font-size:0.9em;}";
    html += ".nw:hover{background:#00d1b2;color:#fff;}"; // Efek hover saat disentuh/diklik
    html += ".rssi{color:#666;font-size:0.8em;} .nw:hover .rssi{color:#fff;}";
    
    // Styling untuk form input dan tombol
    html += "input{width:100%;padding:12px;margin:10px 0;border-radius:5px;border:1px solid #444;background:#333;color:#fff;box-sizing:border-box;}";
    html += "button{width:100%;padding:12px;background:#00d1b2;border:none;color:#fff;font-weight:bold;border-radius:5px;cursor:pointer;margin-top:10px;}";
    html += "button:hover{background:#00b89c;}";
    html += ".footer{margin-top:20px;font-size:0.8em;color:#555;}";
    html += "</style>";
    
    // 3. JAVASCRIPT: Menangani interaksi klik pada daftar WiFi
    html += "<script>";
    // Fungsi ini mengisi input SSID dan memindahkan kursor ke input Password secara otomatis
    html += "function fillSSID(s){ document.getElementsByName('s')[0].value=s; document.getElementsByName('p')[0].focus(); }";
    html += "</script>";
    
    html += "</head><body><div class='card'>";
    html += "<h2>IskakINO Portal</h2>";
    
    // Menampilkan daftar WiFi hasil scan di atas form
    html += "<h3>Select Network</h3>";
    html += "<div class='net-box'>" + wifiList + "</div>";
    
    // 4. FORM CONFIG: Tempat input SSID, Password, dan Parameter Kustom
    html += "<form action='/save' method='POST'>";
    html += "<h3>WiFi Credentials</h3>";
    html += "<input name='s' placeholder='SSID' required>"; // Nama WiFi
    html += "<input name='p' type='password' placeholder='Password'>"; // Sandi WiFi
    
    // Menampilkan Parameter Kustom (jika ada yang didaftarkan oleh pengguna)
    if (_paramsCount > 0) {
        html += "<h3>Custom Parameters</h3>";
        for (int i = 0; i < _paramsCount; i++) {
            // Memberi nama input 'c0', 'c1', dst. sesuai urutan parameter
            html += "<input name='c" + String(i) + "' placeholder='" + _params[i].label + "' value='" + _params[i].value + "'>";
        }
    }
    
    html += "<button type='submit'>SAVE & CONNECT</button>";
    html += "</form>";
    html += "<div class='footer'>IskakINO_WifiPortal v1.0.0</div>";
    html += "</div></body></html>";

    // Kirim seluruh string HTML ke browser klien dengan status HTTP 200 (OK)
    _server.send(200, "text/html", html);
}

void IskakINO_WifiPortal::handleOTA() {
    String html = "<html><body><form method='POST' action='/update' enctype='multipart/form-data'>";
    html += "<h2>OTA Update</h2><input type='file' name='update'><button type='submit'>Update</button></form></body></html>";
    _server->send(200, "text/html", html);
}

// --- Storage Logic ---
bool IskakINO_WifiPortal::loadConfig() {
    #if defined(ESP32)
        Preferences prefs;
        prefs.begin("iskakino", true);
        String s = prefs.getString("ssid", "");
        prefs.end();
        return (s != "");
    #elif defined(ESP8266)
        LittleFS.begin();
        return LittleFS.exists("/iskak.txt");
    #endif
}

void IskakINO_WifiPortal::saveConfig(String ssid, String pass) {
    #if defined(ESP32)
        Preferences prefs;
        prefs.begin("iskakino", false);
        prefs.putString("ssid", ssid);
        prefs.putString("pass", pass);
        // Simpan params juga di sini jika perlu di versi pengembangan berikutnya
        prefs.end();
    #elif defined(ESP8266)
        File f = LittleFS.open("/iskak.txt", "w");
        f.println(ssid);
        f.println(pass);
        f.close();
    #endif
    WiFi.begin(ssid.c_str(), pass.c_str());
}

void IskakINO_WifiPortal::resetSettings() {
    #if defined(ESP32)
        Preferences prefs; prefs.begin("iskakino", false); prefs.clear(); prefs.end();
    #elif defined(ESP8266)
        LittleFS.remove("/iskak.txt");
    #endif
    ESP.restart();
}
