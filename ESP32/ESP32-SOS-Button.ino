#include <WiFi.h>
#include <HTTPClient.h>

// ========================================
// ESP32-C3 SOS BUTTON
// Immediate SOS + built-in blue LED
// ========================================

const char* WIFI_SSID = "YOUR_WIFI_NAME";
const char* WIFI_PASSWORD = "YOUR_WIFI_PASSWORD";
const char* PHONE_IP = "192.168.0.101";

const int BUTTON_PIN = 4;
const int LED_PIN = 8;       // Built-in blue LED

const unsigned long COOLDOWN = 10000;

bool hasSentSOS = false;
unsigned long lastSend = 0;

bool sosConfirmed = false;
unsigned long ledTimer = 0;
bool ledState = false;

void ledOn() {
  digitalWrite(LED_PIN, LOW);       // active LOW
}

void ledOff() {
  digitalWrite(LED_PIN, HIGH);
}

void setup() {
  Serial.begin(115200);
  delay(500);

  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);
  ledOff();

  Serial.println();
  Serial.println("================================");
  Serial.println("      ESP32 SOS BUTTON v5");
  Serial.println("      BUILT-IN BLUE LED");
  Serial.println("================================");

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  Serial.println("[WiFi] Connecting...");

  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 40) {
    // Fast blink while connecting
    digitalWrite(LED_PIN, !digitalRead(LED_PIN));
    delay(250);
    Serial.print(".");
    attempts++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println();
    Serial.println("[WiFi] Connected!");
    Serial.print("[WiFi] IP address: ");
    Serial.println(WiFi.localIP());

    Serial.println();
    Serial.println("[Phone] SOS URL:");
    Serial.print("http://");
    Serial.print(PHONE_IP);
    Serial.println(":8080/sos");

    Serial.println();
    Serial.println("[BUTTON] GPIO: 4");
    Serial.println("[LED] Built-in blue LED GPIO: 8");

    Serial.println();
    Serial.println("--------------------------------");
    Serial.println("[SYSTEM] READY");
    Serial.println("--------------------------------");

    ledOff();
  } else {
    Serial.println();
    Serial.println("[WiFi] ERROR: connection failed");
    ledOn();
  }
}

void loop() {

  if (WiFi.status() != WL_CONNECTED) {
    // Fast blink while reconnecting
    digitalWrite(LED_PIN, !digitalRead(LED_PIN));
    WiFi.reconnect();
    delay(500);
    return;
  }

  // Normal standby:
  // blue LED slowly blinks while Wi-Fi is connected.
  // After successful SOS it stays ON.
  if (!sosConfirmed && millis() - ledTimer >= 1000) {
    ledTimer = millis();
    ledState = !ledState;

    if (ledState)
      ledOn();
    else
      ledOff();
  }

  if (digitalRead(BUTTON_PIN) == LOW) {

    Serial.println();
    Serial.println(">>> BUTTON PRESSED");
    Serial.println(">>> Sending SOS...");

    delay(50);

    if (digitalRead(BUTTON_PIN) == HIGH)
      return;

    if (hasSentSOS && (millis() - lastSend < COOLDOWN)) {
      Serial.println("[SOS] Cooldown active");
      delay(500);
      return;
    }

    Serial.println();
    Serial.println("================================");
    Serial.println(">>> SOS BUTTON ACTIVATED");
    Serial.println("================================");

    bool success = false;

    // LED OFF while sending.
    // It will turn ON permanently only after SOS OK.
    sosConfirmed = false;
    ledOff();

    for (int attempt = 1; attempt <= 3; attempt++) {

      Serial.println();
      Serial.print("[SOS] Attempt ");
      Serial.print(attempt);
      Serial.println("/3");

      if (WiFi.status() != WL_CONNECTED) {
        Serial.println("[SOS] WiFi disconnected, reconnecting...");
        WiFi.reconnect();
        delay(1000);
      }

      HTTPClient http;

      String url = String("http://") + PHONE_IP + ":8080/sos";

      Serial.println("[SOS] Connecting to phone...");
      Serial.println(url);

      http.setTimeout(5000);
      http.begin(url);
      http.addHeader("Connection", "close");

      int httpCode = http.GET();

      Serial.print("[SOS] HTTP code: ");
      Serial.println(httpCode);

      if (httpCode > 0) {

        String response = http.getString();
        response.trim();

        Serial.println("[SOS] Server response:");
        Serial.println(response);

        // This is the actual confirmation from Android.
        if (httpCode == 200 && response.startsWith("SOS OK")) {
          success = true;
          http.end();
          break;
        }

      } else {
        Serial.print("[SOS] HTTP ERROR: ");
        Serial.println(http.errorToString(httpCode));
      }

      http.end();

      if (attempt < 3) {
        Serial.println("[SOS] Retrying in 500 ms...");
        delay(500);
      }
    }

    if (success) {

      hasSentSOS = true;
      lastSend = millis();

      Serial.println();
      Serial.println("################################");
      Serial.println("#       SOS SENT SUCCESSFULLY  #");
      Serial.println("#       PHONE SHOULD CALL      #");
      Serial.println("################################");

      // SUCCESS:
      // blue LED stays permanently ON.
      sosConfirmed = true;
      ledOn();

    } else {

      Serial.println();
      Serial.println("################################");
      Serial.println("#          SOS FAILED          #");
      Serial.println("################################");

      // ERROR: 5 quick flashes.
      for (int i = 0; i < 5; i++) {
        ledOn();
        delay(150);
        ledOff();
        delay(150);
      }

      // Return to normal standby blinking.
      ledState = false;
      ledTimer = millis();
    }

    // Wait until the button is released.
    while (digitalRead(BUTTON_PIN) == LOW) {
      delay(20);
    }

    delay(300);
  }
}
