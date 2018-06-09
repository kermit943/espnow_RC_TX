#include <ESP8266WiFi.h>
#include <U8x8lib.h>
#include <Wire.h>
#include "RC.h"

#include "timecallback.h"

extern "C" {
#include <espnow.h>
}
// this is the MAC Address of the remote ESP
uint8_t remoteMac1[] = {0x60, 0x01, 0x94, 0x29, 0x8C, 0xCF}; // Server MAC
uint8_t remoteMac2[] = {0x60, 0x01, 0x94, 0x29, 0x8C, 0xCF}; // same mac just for tests
uint8_t remoteMac3[] = {0x60, 0x01, 0x94, 0x29, 0x8C, 0xCF}; // same mac just for tests



U8X8_SSD1306_128X32_UNIVISION_HW_I2C u8x8(/* reset=*/ U8X8_PIN_NONE, /* clock=*/ SCL, /* data=*/ SDA);   // pin remapping with ESP8266 HW I2C

const int next = 14;
const int enter = 12;



#define WIFI_CHANNEL 4

volatile boolean recv;
#define TdataSizeMax 32
uint8_t TdataLen;
uint8_t Tdata[TdataSizeMax];

void recv_cb(u8 *macaddr, u8 *data, u8 len)
{
  recv = true;
  //Serial.print("recv_cb ");
  //Serial.println(len);
  if (len <= TdataSizeMax)
  {
    TdataLen = len;
    for (int i = 0; i < len; i++) Tdata[i] = data[i];
  }
};

void send_cb(uint8_t* mac, uint8_t sendStatus)
{
  //Serial.print("send_cb ");
};


float voltage = 0;
const int numReadings = 5;
int readings[numReadings]; // the readings from the analog input
int readIndex = 0;         // the index of the current reading
int total = 0;             // the running totals

float readandsmooth(int Pin) {
  total = total - readings[readIndex];
  readings[readIndex] = analogRead(Pin);
  total = total + readings[readIndex];
  readIndex = readIndex + 1;

  if (readIndex >= numReadings) {
    readIndex = 0;
  }
  return total / numReadings;
}

void ShowData() {

  //  Serial.print(voltage);
  //  Serial.println(" V");


  u8x8.setCursor(3, 0);
  u8x8.print("     ");
  u8x8.setCursor(0, 0);
  u8x8.print("TX:");
  u8x8.print(voltage);
  u8x8.print("V"); // write something to the internal memory
}


void conectEspNow(int modelnr) {

  WiFi.disconnect();

  if (esp_now_init() != 0) Serial.println("*** ESP_Now init failed");

  esp_now_set_self_role(ESP_NOW_ROLE_COMBO);

  if (modelnr == 1) {
    esp_now_add_peer(remoteMac1, ESP_NOW_ROLE_COMBO, WIFI_CHANNEL, NULL, 0);
    Serial.println("conect to MAC1");
  } else if (modelnr == 2) {
    esp_now_add_peer(remoteMac2, ESP_NOW_ROLE_COMBO, WIFI_CHANNEL, NULL, 0);
    Serial.println("conect to MAC1");
  } else if (modelnr == 3) {
    esp_now_add_peer(remoteMac3, ESP_NOW_ROLE_COMBO, WIFI_CHANNEL, NULL, 0);
    Serial.println("conect to MAC1");
  }


  esp_now_register_recv_cb(recv_cb);
  esp_now_register_send_cb(send_cb);

}


int model = 0;
int Model = 0;

bool selectModel() {

  if (digitalRead(next) == 0) {
    model ++;
    delay(100);
    if (model >= 4) {
      model = 1;
    }
  }

  if (model == 0) {
    u8x8.setCursor(0, 2);
    u8x8.print("                ");
    u8x8.setCursor(0, 2);
    u8x8.print("SELECT MODEL");
    delay(100);

  } else if (model == 1) {
    u8x8.setCursor(0, 2);
    u8x8.print("                ");
    u8x8.setCursor(0, 2);
    u8x8.print("MODEL 1");
    Model = 1;
    delay(100);

  } else if (model == 2) {
    u8x8.setCursor(0, 2);
    u8x8.print("                ");
    u8x8.setCursor(0, 2);
    u8x8.print("MODEL 2");
    Model = 2;
    delay(100);

  } else if (model == 3) {
    u8x8.setCursor(0, 2);
    u8x8.print("                ");
    u8x8.setCursor(0, 2);
    u8x8.print("MODEL 3");
    Model = 3;
    delay(100);

  }

  if (digitalRead(enter) == 0 && model != 0) {
    return true;
  } else {
    return false;
  }
}


TimeCallback TimerCallback;

void setup() {

  Serial.begin(115200);
  Serial.println();
  Wire.begin();

  pinMode(next, INPUT_PULLUP);
  pinMode(enter, INPUT_PULLUP);


  u8x8.begin();
  u8x8.clearDisplay();
  u8x8.setFont(u8x8_font_5x7_f);
  u8x8.setCursor(0, 0);
  u8x8.print("Boot");
  delay(600);
  u8x8.clearDisplay();

  WiFi.mode(WIFI_STA); // Station mode for esp-now
  WiFi.disconnect();

  Serial.printf("This mac: %s, ", WiFi.macAddress().c_str());
  Serial.printf(", channel: %i\n", WIFI_CHANNEL);
  //
  //  if (esp_now_init() != 0) Serial.println("*** ESP_Now init failed");
  //
  //  esp_now_set_self_role(ESP_NOW_ROLE_COMBO);
  //  esp_now_add_peer(remoteMac1, ESP_NOW_ROLE_COMBO, WIFI_CHANNEL, NULL, 0);
  //  //esp_now_add_peer(remoteMac2, ESP_NOW_ROLE_COMBO, WIFI_CHANNEL, NULL, 0);
  //
  //  esp_now_register_recv_cb(recv_cb);
  //  esp_now_register_send_cb(send_cb);

  while (1) {
    if (selectModel() == true) {
      break;
    }
  }
  conectEspNow(Model);
  u8x8.setCursor(0, 2);
  u8x8.print("               ");
  u8x8.setCursor(0, 2);
  u8x8.print("Model:");
  u8x8.print(Model);

  delay(600);

  init_RC();

  TimerCallback.addTimer("ShowData", 500, &ShowData);

}

uint32_t lastRC;

void loop()
{

  TimerCallback.timerHandler(); // TimerCallback-Stuff
  voltage = readandsmooth(A0) * (5.5 / 1024.0);

  uint32_t now;
  delay(2);
  now = millis(); // actual time

  if (gotRC)
  {
    gotRC = false;
    rc_to_buf();
    esp_now_send(remoteMac1, RCdata.data, RCdataSize);
    //esp_now_send(NULL, RCdata.data, RCdataSize); // Sent to all peers
    seqno++;

    //Serial.println(now -lastRC);
    lastRC = now;

    //Serial.print(rcValue[0]); Serial.print("  ");
    //Serial.print(rcValue[1]); Serial.print("  ");
    //Serial.print(rcValue[2]); Serial.print("  ");
    //Serial.print(rcValue[3]); Serial.println();

    u8x8.setCursor(8, 2);
    u8x8.print("      ");

  }  else if (now >= lastRC + 100) {

    lastRC = now;
    Serial.println("No PPM input");
    //    u8x8.setCursor(0, 8);
    //    u8x8.print("      ");
    u8x8.setCursor(8, 2);
    u8x8.print("No PPM");
  }

  if (recv)
  {
    recv = false;
    //Serial.write(Tdata,TdataLen);  //
  }

}


