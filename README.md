# ESP32 SOS Button

A simple **ESP32-C3 SOS button** that makes an Android smartphone call a predefined SOS number over its SIM card.

The ESP32 and phone communicate directly over the local Wi-Fi network. No cloud server, Twilio account, or paid API is required.

![ESP32 SOS Button](docs/images/project.jpg)

## How it works

```mermaid
flowchart LR
    A[ESP32-C3 SOS Button] -->|Wi-Fi HTTP /sos| B[Android SOS App]
    B -->|Telecom / SIM| C[SOS Phone Number]
```

1. Press the SOS button.
2. ESP32 immediately sends an HTTP request to the Android phone.
3. The Android app receives `/sos`.
4. The app starts a normal phone call to the saved SOS number using the phone's SIM.
5. The Android server returns `SOS OK`.
6. ESP32 turns its built-in blue LED on continuously to confirm that the call command was accepted.

> **Important:** `SOS OK` confirms that the Android app accepted the request and initiated the call. It does **not** mean that the person on the other end answered the call.

---

## Features

- ESP32-C3 Super Mini
- One physical SOS button
- Immediate SOS activation — no 3-second hold
- Android phone acts as the cellular calling device
- Configurable SOS phone number
- No cloud service required
- Local Wi-Fi communication
- Automatic HTTP retry (up to 3 attempts)
- Built-in blue LED status indication
- 10-second software cooldown after a successful SOS
- Serial Monitor diagnostics

---

## Hardware

### Required

- ESP32-C3 Super Mini
- Push button
- USB cable
- Android smartphone with SIM/mobile service
- Wi-Fi network

### Wiring

The button uses the ESP32 internal pull-up resistor.

```text
ESP32-C3 Super Mini

GPIO 4  ─────── Push Button ─────── GND
```

No external resistor is required for the button.

The built-in blue LED is used for status indication, so no external LED is required.

### LED status

| State | Built-in blue LED |
|---|---|
| Connecting to Wi-Fi | Fast blinking |
| Wi-Fi connected / standby | Slow blinking |
| Sending SOS | OFF |
| `SOS OK` received | **ON continuously** |
| SOS failed | 5 quick flashes, then standby |

---

# Installation

## Step 1 — Install the Android app

The Android application is included here:

**`Android-App/ESP32-SOS-Call.apk`**

Copy the APK to your Android phone and install it.

If Android asks for permission to install apps from this source, enable the corresponding **Allow from this source** option.

Open the app:

![Android App](docs/images/android-app.jpg)

### Configure the SOS number

1. Enter the phone number that should receive the SOS call.
2. Tap **Save Number**.
3. Check **Active SOS number**.
4. Check **Server active number**.
5. Make sure the app shows:

```text
SOS SERVER RUNNING
```

The app also displays the phone's local IP address, for example:

```text
Phone IP address: 192.168.0.101
```

Write this IP down — it will be entered into the ESP32 sketch.

### Test the Android app

Before programming the ESP32, press:

**Test Call**

Make sure the phone successfully starts a call to the configured SOS number.

> Do not test with a real emergency number unless you actually need emergency services. Use a number you control or a test number.

---

# Step 2 — Install Arduino IDE

Install **Arduino IDE** on your computer.

Then install ESP32 board support:

1. Open **Arduino IDE**
2. Go to **File → Preferences**
3. Add the ESP32 Boards Manager URL if ESP32 support is not already installed.
4. Open **Tools → Board → Boards Manager**
5. Search for **esp32**
6. Install **ESP32 by Espressif Systems**

Select:

```text
Board: ESP32C3 Dev Module
```

Then select the COM port of your ESP32-C3.

---

# Step 3 — Configure the ESP32 sketch

Open:

```text
ESP32/ESP32-SOS-Button.ino
```

At the top of the sketch, change these three values:

```cpp
const char* WIFI_SSID = "YOUR_WIFI_NAME";
const char* WIFI_PASSWORD = "YOUR_WIFI_PASSWORD";
const char* PHONE_IP = "192.168.0.101";
```

### Example

If your Wi-Fi network is:

```text
Wi-Fi name: MyHomeWiFi
Wi-Fi password: 12345678
Phone IP: 192.168.0.101
```

use:

```cpp
const char* WIFI_SSID = "MyHomeWiFi";
const char* WIFI_PASSWORD = "12345678";
const char* PHONE_IP = "192.168.0.101";
```

### Important

The ESP32 and the phone must be connected to the **same Wi-Fi network**.

For example:

```text
Phone:  192.168.0.101
ESP32:  192.168.0.102
Router: 192.168.0.1
```

The phone IP can change after reconnecting to Wi-Fi. If it changes, update `PHONE_IP` in the sketch and upload it again.

---

# Step 4 — Upload the ESP32 firmware

Connect the ESP32-C3 to the computer using USB.

In Arduino IDE:

```text
Tools → Board → ESP32C3 Dev Module
Tools → Port → your ESP32 COM port
```

Click:

**Upload**

After the upload is complete, open:

**Tools → Serial Monitor**

Set the baud rate to:

```text
115200
```

You should see something similar to:

```text
================================
      ESP32 SOS BUTTON v5
      BUILT-IN BLUE LED
================================
[WiFi] Connecting...
[WiFi] Connected!
[WiFi] IP address: 192.168.0.102

[Phone] SOS URL:
http://192.168.0.101:8080/sos

[BUTTON] GPIO: 4
[LED] Built-in blue LED GPIO: 8

--------------------------------
[SYSTEM] READY
--------------------------------
```

---

# Step 5 — Test the SOS button

Press the physical button.

The ESP32 sends the request immediately.

The Serial Monitor should show:

```text
>>> BUTTON PRESSED
>>> Sending SOS...

================================
>>> SOS BUTTON ACTIVATED
================================

[SOS] Attempt 1/3
[SOS] Connecting to phone...
http://192.168.0.101:8080/sos
[SOS] HTTP code: 200
[SOS] Server response:
SOS OK
Calling: 67676767
```

Then:

```text
################################
#       SOS SENT SUCCESSFULLY  #
#       PHONE SHOULD CALL      #
################################
```

The Android phone should now start the call.

The ESP32's built-in blue LED will remain **ON**.

---

# Troubleshooting

## `connection refused`

If Serial Monitor shows:

```text
HTTP ERROR: connection refused
```

check:

- Android app is open/running.
- The app shows **SOS SERVER RUNNING**.
- Phone and ESP32 are on the same Wi-Fi.
- `PHONE_IP` matches the IP shown in the Android app.
- Port `8080` is used.

The firmware automatically retries the request up to 3 times.

---

## Phone IP changed

If the app now shows:

```text
Phone IP address: 192.168.0.105
```

but the sketch still contains:

```cpp
const char* PHONE_IP = "192.168.0.101";
```

change it to:

```cpp
const char* PHONE_IP = "192.168.0.105";
```

Then upload the sketch again.

---

## ESP32 does not connect to Wi-Fi

Check:

- Wi-Fi name is correct.
- Wi-Fi password is correct.
- The network is available.
- The ESP32 is within Wi-Fi range.

The built-in blue LED flashes quickly while the ESP32 is trying to connect.

---

## Android app does not make the call

First use the app's **Test Call** button.

If Test Call works but the ESP32 does not:

1. Check the phone IP.
2. Check `PHONE_IP` in the ESP32 sketch.
3. Check that both devices are on the same Wi-Fi.
4. Check Serial Monitor for `HTTP 200` and `SOS OK`.

---

# Project Structure

```text
ESP32-SOS-Button/
│
├── ESP32/
│   └── ESP32-SOS-Button.ino
│
├── Android-App/
│   └── ESP32-SOS-Call.apk
│
├── docs/
│   └── images/
│       ├── project.jpg
│       ├── android-app.jpg
│       └── serial-monitor.png
│
├── README.md
├── README_RU.md
├── LICENSE
└── .gitignore
```

---

# Technology

### ESP32

- ESP32-C3 Super Mini
- Arduino framework
- Wi-Fi
- HTTP
- GPIO button
- Built-in LED

### Android

- Android application
- Local HTTP server
- Android Telecom API
- Cellular SIM call

### Communication

```text
ESP32-C3
   │
   │ HTTP GET
   ▼
http://PHONE_IP:8080/sos
   │
   ▼
Android SOS Server
   │
   │ Telecom API
   ▼
Cellular Call
   │
   ▼
SOS Number
```

---

# Safety note

This project is intended as an electronics/DIY demonstration.

Do not rely on it as your only emergency communication system without thoroughly testing the complete setup under the conditions where it will be used.

The project depends on:

- ESP32 power
- Wi-Fi availability
- Android phone power
- Android app/server availability
- cellular network availability
- correct phone number configuration

Always keep an appropriate conventional emergency communication method available.

---

# License

MIT License

See `LICENSE`.
