#BTC Price & CDC Action Zone Display (ESP32 T-Display S3)

🎯 Ideal Use Case
This project is ideally suited for individuals looking for an effective way to identify signals for accumulating Bitcoin (BTC) as a savings strategy. It leverages the CDC Action Zone buy signals, a popular technical analysis indicator widely associated with the teachings of Lecturer Tumm Piriya the "Piranha" methodology in Thailand. By visualizing these key indicators, users can potentially make more informed decisions regarding their BTC accumulation efforts.

## Display Preview
![BTC Price & CDC Action Zone on LilyGo T-Display S3](images/cdc_display.jpg)

This project displays the current Bitcoin (BTC) price and the CDC Action Zone chart on the TFT screen of the LilyGo T-Display S3 board (or any ESP32 with a TFT screen that supports TFT_eSPI) by fetching price data from the Binance API.

---

## 💡 Features

* **Displays Current BTC Price:** Fetches the latest price data from the Binance API and displays it on the screen.
* **CDC Action Zone Chart:** Shows the EMA 12 and EMA 26 lines, along with colored zones (green/red) and Golden Cross / Death Cross indicators for simple trend analysis.
* **Automatic Updates:** Data is fetched and refreshed every 5 minutes.
* **Power Saving Mode:** Press the button on the board (GPIO 14) to enter Deep Sleep mode for power saving.
* **Automatic Wi-Fi Connection:** Uses `WiFiManager` for easy Wi-Fi setup via a captive portal.

## 🛠️ Hardware Requirements

* **ESP32 Board:** Recommended **LilyGo T-Display S3** (or any ESP32 board with a TFT display that supports the TFT_eSPI library).
* USB-C cable for code uploading and power.

## 🚀 Setup

### 1. Install Arduino IDE

If you don't have it already, download and install the Arduino IDE from the [official Arduino website](https://www.arduino.cc/en/software).

### 2. Install ESP32 Board Support

Add the ESP32 Board Manager URL to the Arduino IDE:
* Open the Arduino IDE and go to `File > Preferences` (or `Arduino > Preferences` on macOS).
* In the `Additional Board Managers URLs:` field, add this URL:
    `https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json`
* Click `OK`.

Then, go to `Tools > Board > Board Manager...`, search for "esp32", and install `esp32 by Espressif Systems`.

### 3. Install Required Libraries

Go to `Sketch > Include Library > Manage Libraries...` and search for and install the following libraries:

* **TFT_eSPI** by Bodmer
* **WiFiManager** by tzapu
* **ArduinoJson** by Benoît Blanchon
* **HTTPClient** (comes with ESP32 Board Support)

### 4. Upload the Code

1.  Open the `.ino` file in the Arduino IDE.
2.  Select the correct board: Go to `Tools > Board` and select `ESP32S3 Dev Module` (or the closest equivalent for your LilyGo T-Display S3).
3.  Select the correct serial port: Go to `Tools > Port` and select the port your ESP32 is connected to.
4.  Click the `Upload` button (right arrow) to compile and upload the code to your board.

## ⚙️ Usage

Once the code is uploaded, the board will start and attempt to connect to Wi-Fi.

1.  **First-time Wi-Fi Setup:** If it's the first time using the device, or if the previously connected Wi-Fi is unavailable, the board will create a Wi-Fi Access Point (AP) named `BTC-Display-Setup`.
    * Connect your phone or computer to this Wi-Fi AP.
    * A captive portal for Wi-Fi configuration should automatically open in your browser (if not, go to `192.168.4.1` in your browser).
    * Select your Wi-Fi network, enter the password, and press `Save`.
    * The board will reboot and connect to the configured Wi-Fi network.

2.  **Display:** After successfully connecting to Wi-Fi, the screen will display the following:
    * Top-left: `BTC` symbol.
    * Top-right: The current Bitcoin price (fetched from Binance).
    * Center: The CDC Action Zone chart showing the EMA 12 (dark green line) and EMA 26 (dark red line), along with historical BTC prices (white line) and Golden Cross (green circle) and Death Cross (red circle) indicators.
    * Bottom: The current date and time.

3.  **Power Saving Mode:** To turn off the screen and enter Deep Sleep mode, briefly press and hold the designated button (`BUTTON_PIN`, which is GPIO 14 on the T-Display S3). The board will display "Shutting down..." and then enter Deep Sleep. Pressing the button again will wake the board.

## ⚠️ Troubleshooting

* **"Failed to get data!"**: Check your board's internet connection, or there might be a temporary issue with fetching data from the Binance API. Try resetting the board.
* **Blank Screen/No Display:** Verify that you have selected the correct board and port when uploading the code, and check the hardware connections.

---
## Support This Project

If you find this project useful or it helps you, please consider supporting its development. Your contributions, however small, are greatly appreciated and help keep the project alive and growing!

### ⚡ Donate with Bitcoin Lightning ⚡

You can send Satoshis directly to my Lightning Wallet:

**Lightning Address:** `slenderswitch67@walletofsatoshi.com`

**Or simply scan the QR Code:**
![Bitcoin Lightning Donation QR Code](images/lightning_donation_qr.png)

**Thank you for every single satoshi you contribute! Your support means a lot.** 🙏

---

## License

This project is licensed under the **Creative Commons Attribution-NonCommercial 4.0 International Public License (CC BY-NC 4.0)**.

This means you are free to:
* **Share** — copy and redistribute the material in any medium or format.
* **Adapt** — remix, transform, and build upon the material.

Under the following terms:
* **Attribution (BY)** — You must give appropriate credit, provide a link to the license, and indicate if changes were made. You may do so in any reasonable manner, but not in any way that suggests the licensor endorses you or your use.
* **NonCommercial (NC)** — You may **not** use the material for commercial purposes. This means you cannot use it to generate revenue.

For the full legal code, please refer to the `LICENSE` file in this repository or visit [Creative Commons BY-NC 4.0](https://creativecommons.org/licenses/by-nc/4.0/legalcode).