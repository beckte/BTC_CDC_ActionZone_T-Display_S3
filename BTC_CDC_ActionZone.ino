// โปรเจกต์แสดงกราฟ BTC (เวอร์ชันแก้ไข Compilation Error)

#include <FS.h>
#include <SPIFFS.h>
#include <WiFi.h>
#include <WiFiManager.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "time.h"
#include <TFT_eSPI.h>

// --- การตั้งค่าปุ่ม ---
#define BUTTON_PIN 14 // ปุ่มด้านบนของ T-Display S3 คือ GPIO 14

// --- การตั้งค่าตัวจับเวลา ---
unsigned long lastUpdateTime = 0;
const long updateInterval = 300000; // 5 นาที

// --- การตั้งค่าอื่นๆ ---
const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 7 * 3600; 
const int daylightOffset_sec = 0;
String historicalApiUrl = "https://api.binance.com/api/v3/klines?symbol=BTCUSDT&interval=1d&limit=31";
String currentPriceApiUrl = "https://api.binance.com/api/v3/ticker/price?symbol=BTCUSDT";

// --- ออบเจ็กต์และตัวแปร ---
TFT_eSPI tft = TFT_eSPI();
float currentPrice = 0;
float historicalPrices[31];
float ema12[31];
float ema26[31];

// --- กลุ่มฟังก์ชันวาดและคำนวณ (ย้ายมาไว้ข้างบน) ---

void drawTickerSymbol() {
  tft.setTextDatum(TL_DATUM);
  tft.setTextColor(TFT_ORANGE);
  tft.setTextSize(3);
  tft.drawString("BTC", 5, 5);
}

void calculateCDCActionZone() {
  float multiplier12 = 2.0 / (12 + 1);
  float multiplier26 = 2.0 / (26 + 1);
  ema12[0] = historicalPrices[0];
  ema26[0] = historicalPrices[0];
  for (int i = 1; i < 31; i++) {
    ema12[i] = (historicalPrices[i] - ema12[i-1]) * multiplier12 + ema12[i-1];
    ema26[i] = (historicalPrices[i] - ema26[i-1]) * multiplier26 + ema26[i-1];
  }
}

void drawChart() {
  float minPrice = 1e9, maxPrice = 0;
  for (int i = 0; i < 31; i++) {
    if (historicalPrices[i] < minPrice) minPrice = historicalPrices[i];
    if (historicalPrices[i] > maxPrice) maxPrice = historicalPrices[i];
  }
  int chartHeight = 80;
  int chartY = 50;
  for (int i = 0; i < 31; i++) {
    int x = map(i, 0, 30, 0, tft.width() - 1);
    int y1 = map(ema12[i], minPrice, maxPrice, chartY + chartHeight, chartY);
    int y2 = map(ema26[i], minPrice, maxPrice, chartY + chartHeight, chartY);
    uint16_t zoneColor = (ema12[i] > ema26[i]) ? TFT_DARKGREEN : TFT_MAROON;
    if (y1 > y2) tft.drawFastVLine(x, y2, y1 - y2, zoneColor);
    else tft.drawFastVLine(x, y1, y2 - y1, zoneColor);
  }
  for (int i = 1; i < 31; i++) {
    int x1 = map(i - 1, 0, 30, 0, tft.width() - 1);
    int y1 = map(historicalPrices[i-1], minPrice, maxPrice, chartY + chartHeight, chartY);
    int x2 = map(i, 0, 30, 0, tft.width() - 1);
    int y2 = map(historicalPrices[i], minPrice, maxPrice, chartY + chartHeight, chartY);
    tft.drawLine(x1, y1, x2, y2, TFT_WHITE);
  }
  for (int i = 1; i < 31; i++) {
    bool goldenCross = ema12[i] > ema26[i] && ema12[i-1] <= ema26[i-1];
    bool deathCross = ema12[i] < ema26[i] && ema12[i-1] >= ema26[i-1];
    if (goldenCross) {
      int x = map(i, 0, 30, 0, tft.width() - 1);
      int y = map(historicalPrices[i], minPrice, maxPrice, chartY + chartHeight, chartY);
      tft.fillCircle(x, y, 3, TFT_GREEN);
    } 
    else if (deathCross) {
      int x = map(i, 0, 30, 0, tft.width() - 1);
      int y = map(historicalPrices[i], minPrice, maxPrice, chartY + chartHeight, chartY);
      tft.fillCircle(x, y, 3, TFT_RED);
    }
  }
}

void drawPriceAndTime() {
  tft.setTextDatum(TR_DATUM); 
  tft.setTextColor(TFT_YELLOW, TFT_BLACK);
  tft.setTextSize(3);
  char priceStr[20];
  sprintf(priceStr, "$%.0f", currentPrice); 
  tft.drawString(priceStr, tft.width() - 5, 5);

  struct tm timeinfo;
  if (getLocalTime(&timeinfo)) {
    char timeStr[30];
    strftime(timeStr, sizeof(timeStr), "%d %b %Y - %H:%M:%S", &timeinfo);
    tft.setTextDatum(BC_DATUM);
    tft.setTextColor(TFT_CYAN, TFT_BLACK);
    tft.setTextSize(2);
    tft.drawString(timeStr, tft.width() / 2, tft.height() - 5);
  }
}

// --- กลุ่มฟังก์ชันจัดการการเชื่อมต่อและข้อมูล ---

bool fetchBTCData() {
  HTTPClient http;
  http.begin(historicalApiUrl);
  int httpCode = http.GET();
  if (httpCode == 200) {
    DynamicJsonDocument doc(12288);
    if (deserializeJson(doc, http.getStream())) { return false; }
    JsonArray klines = doc.as<JsonArray>();
    if (klines.size() < 31) { return false; }
    for (int i = 0; i < 31; i++) {
      historicalPrices[i] = klines[i][4].as<float>();
    }
  } else {
    return false;
  }
  http.end();

  http.begin(currentPriceApiUrl);
  httpCode = http.GET();
  if (httpCode == 200) {
    DynamicJsonDocument doc(1024);
    if (deserializeJson(doc, http.getStream())) { return false; }
    currentPrice = doc["price"].as<float>();
  } else {
    return false;
  }
  http.end();
  return true;
}

void updateAndDraw() {
  tft.fillScreen(TFT_BLACK);
  tft.setTextSize(2);
  tft.setTextColor(TFT_WHITE);
  tft.drawString("Fetching data...", 40, tft.height() / 2 - 10);

  if (fetchBTCData()) {
    tft.fillScreen(TFT_BLACK);
    drawTickerSymbol(); 
    calculateCDCActionZone();
    drawChart();
    drawPriceAndTime();
  } else {
    tft.fillScreen(TFT_BLACK);
    drawTickerSymbol(); 
    tft.drawString("Failed to get data!", 40, tft.height() / 2 - 10);
  }
}

void connectWiFi() {
  tft.setTextSize(2);
  tft.setTextColor(TFT_WHITE);
  tft.drawString("Connecting WiFi...", 5, tft.height() / 2 - 10);
  WiFiManager wm;
  if (!wm.autoConnect("BTC-Display-Setup")) {
    tft.drawString("WiFi Failed!", 5, tft.height() / 2 + 10);
    while(true);
  }
}

// ------------------- SETUP -------------------
void setup() {
  Serial.begin(115200);
  if(!SPIFFS.begin(true)){
    return;
  }
  
  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);
  
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  esp_sleep_enable_ext0_wakeup(GPIO_NUM_14, 0); 

  connectWiFi();

  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  
  updateAndDraw();
}

// ------------------- LOOP -------------------
void loop() {
  if (digitalRead(BUTTON_PIN) == LOW) {
    delay(50); 
    if (digitalRead(BUTTON_PIN) == LOW) {
      tft.fillScreen(TFT_BLACK);
      tft.setTextDatum(MC_DATUM);
      tft.drawString("Shutting down...", tft.width() / 2, tft.height() / 2);
      delay(1000);
      tft.writecommand(ST7789_DISPOFF); 
      
      esp_deep_sleep_start();
    }
  }

  if (millis() - lastUpdateTime >= updateInterval) {
    lastUpdateTime = millis();
    updateAndDraw();
  }
}