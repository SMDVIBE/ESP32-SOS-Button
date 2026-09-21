# ESP32 SOS Button — Project Info

**Main board:** ESP32-C3 Super Mini  
**Android app:** ESP32 SOS CALL  
**Communication:** Local Wi-Fi / HTTP  
**HTTP endpoint:** `GET /sos` on port `8080`  
**Button:** GPIO4 to GND (`INPUT_PULLUP`)  
**Built-in LED:** GPIO8, active LOW  
**SOS activation:** Immediate press, no 3-second hold  
**Successful response:** HTTP 200 + `SOS OK`  
**Retry:** Up to 3 attempts  
**Cooldown:** 10 seconds after a successful SOS
