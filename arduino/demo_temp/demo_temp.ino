#include "EspMQTTClient.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define ONE_WIRE_BUS 23

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

#define SCREEN_WIDTH 128 
#define SCREEN_HEIGHT 32 
#define SCREEN_ADDRESS 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

float temp = 0;

EspMQTTClient client(
  "Vinci's Workshop",
  "asdfzxcv",
  "159.203.2.152",  // MQTT Broker server ip
  "sensi",   // Can be omitted if not needed
  "password",   // Can be omitted if not needed
  "Temperature",     // Client name that uniquely identify your device
  1883              // The MQTT port, default to 1883. this line can be omitted
);

void setup()
{
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  display.setRotation(2);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(20,20);
  display.println("BOOTING UP");
  display.display();
  sensors.begin();
  Serial.begin(115200);
  client.enableDebuggingMessages(); // Enable debugging messages sent to serial output
  client.enableLastWillMessage("sensi/demo/Temperature_Sensor/events", "disconnected");
}

void onConnectionEstablished()
{
  client.subscribe("sensi/demo/Temperature_Sensor", [](const String & payload) {
    Serial.println(payload);
  });
  client.publish("sensi/demo/Temperature_Sensor/events", "connected");
  display.clearDisplay();
}


void loop()
{
  sensors.requestTemperatures();
  temp = sensors.getTempCByIndex(0);
  client.loop();
  delay(2000);
  client.publish("sensi/demo/Temperature_Sensor/telemetry/temperature", String(temp));

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(5,5);
  display.print("T: ");
  display.println(String(temp));
  display.display();
  delay(100);
  display.clearDisplay();
}
