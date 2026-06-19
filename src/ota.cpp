// OTA disabled for v1 — stub only
#include "ota.h"

void OtaManager::begin(const char*, const char*) {
    Serial.println("[OTA] Disabled in v1 build.");
}

void OtaManager::handle() {}

String OtaManager::ipAddress() const {
    return "OTA disabled";
}
