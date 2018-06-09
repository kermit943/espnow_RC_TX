#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

/**
 * Wichtig: Die cpp includen
 */
#include "lib/timecallback.cpp"

const char *ssid = "";
const char *password = "";
const char *chip_name = "";

/**
 * Timer TimerCallback Instanz laden
 */
TimeCallback TimerCallback;

void fooFunc() {
  Serial.println("Default function every second");
}

void setup() {

  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  ArduinoOTA.setHostname(chip_name);
  ArduinoOTA.begin();

  /**
   * Beispiel: Timer mit Referenz auf eine Funktion
   *
   * Param 1: test1 ist hier der name
   * Param 2: 1000ms => Sekündlich
   * Param 3: Referenz auf Funktion
   */
  TimerCallback.addTimer("test1", 1000, &fooFunc);

  /**
   * Beispiel: Timer mit Inline-Funktion
   *
   * Param 1: test2 ist hier der name
   * Param 2: 2000ms => Alle 2 Sekunden
   * Param 3: Funktion die ausgeführt wird
   */
  TimerCallback.addInlineTimer("test2", 2000, [](){
    Serial.println("Inline function 2 seconds");
  });

  /**
   * Beispiel: Timer test1 soll erstellt werden, aber der name existiert bereits
   */
  if(!TimerCallback.addTimer("test1", 1000, &fooFunc)) {
    Serial.println("Timer test1 already exist");
  }

  /**
   * Beispiel: Timer mit dem name test1 löschen
   */
  if(TimerCallback.removeTimer("test1")) {
    Serial.println("Timer test1 was removed successfully");
  }

  /**
   * Beispiel: Timer test1 soll erstellt werden, nun ist der name wieder frei
   */
  if(!TimerCallback.addTimer("test1", 1000, &fooFunc)) {
    Serial.println("Timer test1 already exist");
  }
}

void loop() {
  ArduinoOTA.handle();

  /**
   * Der Handler muss im Loop ausgeführt werden!
   */
  TimerCallback.timerHandler();
}
