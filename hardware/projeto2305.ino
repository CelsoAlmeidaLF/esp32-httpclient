
/* ########################################################################
 * # PROJETO:        WIFI - HTTPClient (API: GET/POST) - VERSÃO:  3.0.0 
 * # AUTOR:          Celso Almeida Leite F
 * # DATA CRIAÇÃO:   01/06/2023
 * # DATA ALTERAÇÃO: 13/06/2023
 * # HARDWARE:       ESP32 Dev Module
 * ######################################################################## */

#include <WiFi.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <String.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

/* ==================================== DEFINE DISPLAY =========================================================== */
#define SCREEN_WIDTH 128     // OLED display width, in pixels
#define SCREEN_HEIGHT 64     // OLED display height, in pixels
#define OLED_RESET -1        // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C  ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

/* ==================================== Pinagem Ports IO ========================================================= */
int D15 = 15;
int D23 = 23;

/* ==================================== Config Wifi ============================================================== */
char ssid[] //  your network SSID (name)
char pass[] // your network password

/* ==================================== Variaveis API ============================================================ */
int code;
String body;
String url;
String contentType = "application/json";
String payload;
String UID = "c9d57d4a-d1e3-45d2-853e-1caee2061967";

/* ==================================== CREATE IMAG ============================================================== */
#define BMP_WIDTH 32
#define BMP_HEIGHT 32

const unsigned char PROGMEM wifi_bmp [] = {
    B00000000,B00000000,B00000000,B00000000,
    B00000000,B00000000,B00000000,B00000000,
    B00000000,B00000011,B11000000,B00000000,
    B00000000,B00111111,B11111100,B00000000,
    B00000001,B11111111,B11111111,B10000000,
    B00000011,B11111111,B11111111,B11100000,
    B00001111,B11100000,B00000111,B11110000,
    B00011111,B10000000,B00000000,B11111000,
    B00111110,B00000000,B00000000,B01111110,
    B01111000,B00011111,B11111000,B00011111,
    B11110000,B01111111,B11111110,B00001111,
    B11100000,B11111111,B11111111,B10000111,
    B11000011,B11111000,B00001111,B11000011,
    B00000111,B11000000,B00000011,B11100000,
    B00001111,B10000000,B00000001,B11110000,
    B00001111,B00000111,B11100000,B11110000,
    B00000110,B00011111,B11111000,B01110000,
    B00000000,B00111111,B11111100,B00000000,
    B00000000,B01111100,B00111110,B00000000,
    B00000000,B01110000,B00001110,B00000000,
    B00000000,B01100001,B10000110,B00000000,
    B00000000,B00000111,B11100000,B00000000,
    B00000000,B00000111,B11100000,B00000000,
    B00000000,B00001111,B11110000,B00000000,
    B00000000,B00001111,B11110000,B00000000,
    B00000000,B00000111,B11100000,B00000000,
    B00000000,B00000111,B11100000,B00000000,
    B00000000,B00000001,B10000000,B00000000,
    B00000000,B00000000,B00000000,B00000000,
    B00000000,B00000000,B00000000,B00000000,
    B00000000,B00000000,B00000000,B00000000
};

/* ############################################### CREATE API #################################################### */

void HandlerGet() {
  HTTPClient api;
  url = "http://192.168.2.62:3000/api/v1";
  api.begin(url.c_str());
  api.addHeader("Content-Type", contentType);
  code = api.GET();
  if (code > 0) {
    delay(3);
    payload = api.getString();
    CodeStatusSucess();
  }else{
    CodeStatusError();
  }
  api.end();
}

void HandlerPost() {
  HTTPClient api;
  url = "http://192.168.2.62:3000/api/v1";
  api.begin(url.c_str());
  api.addHeader("Content-Type", contentType);
  body = CreateJson();
  code = api.POST(body);
  if (code > 0) {
    delay(3);
    payload = api.getString();
    CodeStatusSucess();
  }else{
    CodeStatusError();
  }
  api.end();
}

void CodeStatusSucess() {
    DisplayPrint("code: 200");
    DisplayPrint(payload);
    delay(4000);
}

void CodeStatusError() {
    DisplayPrint("code: 404");
    DisplayPrint("API: not found ...");
    delay(3000);
}

String CreateJson() {
  String vJson = "";
  StaticJsonDocument<100> doc;
  doc["hardware"] = "esp32";
  doc["uid"] = UID;
  serializeJson(doc, vJson);
  Serial.println(vJson);
  return vJson;
}

/* ############################################### CONFIG DISPLAY ################################################# */

void DisplayGrafic(){
  display.drawBitmap((display.width() - BMP_WIDTH)/2, 8, wifi_bmp, BMP_WIDTH, BMP_HEIGHT, 1);
  display.display();
}

void DisplayClear(){
  display.clearDisplay();
  display.setCursor(0, 0);
}

void DisplayPrint(String msg) {
  display.println(msg);
  display.display();
}

/* ############################################### METHODS: PROGRAM ############################################### */

bool IsValid(bool in) {
  return in == 1;
}

/* ############################################### INIT PROGRAM ################################################### */

void setup() {

  pinMode(D15, INPUT);
  pinMode(D23, INPUT);

  Serial.begin(115200);

  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ;  // Don't proceed, loop forever
  }

  display.display();

  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  delay(2000);

  delay(10);
  DisplayClear();
  DisplayPrint("Connecting to ");
  DisplayPrint(ssid);

  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    DisplayClear();
    display.print(F("."));
    delay(30);
  }

  IPAddress ip = WiFi.localIP();
  DisplayClear();
  DisplayPrint("WiFi connected");
  DisplayPrint("IP address: ");
  display.println(ip);
  display.display();
}

void loop() {
 
  bool btn1 = digitalRead(D15);
  bool btn2 = digitalRead(D23);

  if (IsValid(btn1)) {
    DisplayClear();
    DisplayPrint("method: GET");
    HandlerGet();
  }

  if (IsValid(btn2)) {
    DisplayClear();
    DisplayPrint("method: POST");
    HandlerPost();
  }

  delay(300);
  DisplayClear();
  DisplayGrafic();
  display.setCursor(12, 54);
  display.print("Wifi  connected");
  display.println("...");
  display.display();
}

/* ############################################### END PROGRAM #################################################### */
