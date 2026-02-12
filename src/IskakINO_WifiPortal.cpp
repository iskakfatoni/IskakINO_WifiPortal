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
                if (!Update.begin(UPDATE_SIZE_UNKNOWN)) Update.printError(Serial);
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
    info += "Chip: " + String(ESP.getChipModel()) + "<br>";
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
        _server->handleClient();
        
        if (_timeout > 0 && (millis() - _portalStartTime > _timeout * 1000)) {
            Serial.println("[IskakINO] Timeout! Restarting...");
            ESP.restart();
        }
    }
}

// Implementasi Storage & Misc Functions...
void IskakINO_WifiPortal::setBrandName(const char* name) { _brandName = name; }
void IskakINO_WifiPortal::enableOTA(bool status) { _otaEnabled = status; }
void IskakINO_WifiPortal::setPortalTimeout(int sec) { _timeout = sec; }

void IskakINO_WifiPortal::handleRoot() { _server->send(200, "text/html", buildHTML()); }
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
