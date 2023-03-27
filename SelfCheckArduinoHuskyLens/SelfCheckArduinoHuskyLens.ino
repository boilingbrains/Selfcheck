#include "HUSKYLENS.h"
#include "SoftwareSerial.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1351.h>
#include <Adafruit_SPITFT.h>
#include <SPI.h>

HUSKYLENS huskylens;
SoftwareSerial mySerial(4, 5); // RX, TX
#define SCREEN_WIDTH  128
#define SCREEN_HEIGHT 128
const uint8_t   OLED_pin_scl_sck        = 13;
const uint8_t   OLED_pin_sda_mosi       = 11;
const uint8_t   OLED_pin_cs_ss          = 10;
const uint8_t   OLED_pin_res_rst        = 9;
const uint8_t   OLED_pin_dc_rs          = 8;
int pin_green_button = 7;
int pin_red_button = 2;
Adafruit_SSD1351 display =
    Adafruit_SSD1351(
        SCREEN_WIDTH,
        SCREEN_HEIGHT,
        &SPI,
        OLED_pin_cs_ss,
        OLED_pin_dc_rs,
        OLED_pin_res_rst
     );

float total = 0;
int last_result_ID = 0;
bool buying = false;
bool last_state_green_button = 0;
int cursor_y = 0; 

void setup() {
  Serial.begin(115200);
  mySerial.begin(9600);
  Wire.begin();
  pinMode(pin_green_button, INPUT);
  pinMode(pin_red_button, INPUT);
  display.begin();
  display.fillScreen(0);
  display.setCursor(0, 60);
  display.setTextColor(0xFFFF, 0);
  display.setTextSize(2);
  display.setCursor(35, 30);
  display.println("Press");
  display.setCursor(35, 60);
  display.setTextColor(0x07E0,0);
  display.println("Green");
  display.setTextColor(0xFFFF, 0);
  display.setCursor(25, 90);
  display.println("To scan");
  while (!huskylens.begin(mySerial))
  {
    Serial.println(F("Begin failed!"));
    Serial.println(F("1.Please recheck the \"Protocol Type\" in HUSKYLENS (General Settings>>Protocol Type>>Serial 9600)"));
    Serial.println(F("2.Please recheck the connection."));
    delay(100);
  }
  setNewName("Tomato: 2.50eu ", 1);
  setNewName("cheese: 3.50eu ", 2);
  setNewName("Pepper: 2.00eu ", 3);

}

void loop() {
  int state_red_button = digitalRead(pin_red_button);
  int state_green_button = digitalRead(pin_green_button);

  if (state_green_button == 1 && !buying && !last_state_green_button) {
    cursor_y = 0; 
    total = 0;
    last_result_ID = 0;
    buying = true;
    display.fillScreen(0);
    display.setTextSize(2);
    display.setTextColor(0x07FF);
    display.setCursor(5, 60);
    display.println("Scan Items");
    delay(1000);
    display.fillScreen(0);
  } else if (state_red_button == 1 && buying) {
    buying = false;
    last_state_green_button = 0;
    printTotal();
    delay(3000);
    display.fillScreen(0);
    total=0;
    cursor_y = 0; 
  }

  if (buying) {
    if (!huskylens.request()) Serial.println(F("Fail to request data from HUSKYLENS, recheck the connection!"));
    else if (!huskylens.isLearned()) Serial.println(F("Nothing learned, press learn button on HUSKYLENS to learn one!"));
    else if (!huskylens.available()) Serial.println(F("No block or arrow appears on the screen!"));
    else {
      HUSKYLENSResult result = huskylens.read();
      if (result.ID == 1 && last_result_ID != 1) {
        last_result_ID = 1;
        total += 2.50;
        display.setTextSize(1);
        display.setCursor(0, cursor_y);
        display.setTextColor(0xFFFF);
        display.println("Tomato added");
        cursor_y += 8;  
        if (cursor_y >= 100) {  
          cursor_y = 0;
        }
      } else if (result.ID == 2 && last_result_ID != 2) {
        last_result_ID = 2;
        total += 3.50;
        display.setTextSize(1);
        display.setCursor(0, cursor_y);
        display.setTextColor(0xFFFF);
        display.println("Cheese added");
        cursor_y += 8;  
        if (cursor_y >= 100) {  
          cursor_y = 0;
        }
      } else if (result.ID == 3 && last_result_ID != 3) {
        last_result_ID = 3;
        total += 2.00;
        display.setTextSize(1);
        display.setCursor(0, cursor_y);
        display.setTextColor(0xFFFF);
        display.println("Pepper added");
        cursor_y += 8;  
        if (cursor_y >= 100) {  
          cursor_y = 0;
        }
      }
      display.setTextSize(1);
      display.setTextColor(0xFFFF);
      display.setCursor(60, 120);
      display.println("Scanning...");
      display.fillRect(0, 120, 50, 8, 0x0000); // Clear the previous total value on the OLED screen
      display.setTextSize(1);
      display.setTextColor(0xFFFF);
      display.setCursor(0, 120);
      display.print("Tot:");
      display.setCursor(20, 120);
      display.setTextColor(0xFFE0);
      display.println(String(total)+F("e"));
    }
  }else{
    display.setCursor(0, 60);
    display.setTextColor(0xFFFF, 0);
    display.setTextSize(2);
    display.setCursor(35, 30);
    display.println("Press");
    display.setCursor(35, 60);
    display.setTextColor(0x07E0,0);
    display.println("Green");
    display.setTextColor(0xFFFF, 0);
    display.setCursor(25, 90);
    display.println("To scan");
  }
}

void setNewName(String newname, uint8_t ID) {
  while (!huskylens.setCustomName(newname, ID))
  {
    Serial.println(F("Custom name failed!"));
    delay(100);
  }
}

void printTotal() {
  display.fillScreen(0);
  display.setCursor(0, 0);
  display.setTextColor(0xF800);
  display.setTextSize(2);
  display.println("You've to pay:");
  display.setCursor(0, SCREEN_HEIGHT / 2 + 10);
  display.setTextSize(3);
  display.setTextColor(0xFFE0, 0);
  display.println(String(total)+F("e"));
}