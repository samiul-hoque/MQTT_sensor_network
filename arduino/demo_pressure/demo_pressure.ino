#include "EspMQTTClient.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_BMP085.h>


Adafruit_BMP085 bmp;

#define SCREEN_WIDTH 128 
#define SCREEN_HEIGHT 32 
#define SCREEN_ADDRESS 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

int pressure;
float temp;
float alt;

EspMQTTClient client(
  "Vinci's Workshop",
  "asdfzxcv",
  "159.203.2.152",  // MQTT Broker server ip
  "sensi",   // Can be omitted if not needed
  "password",   // Can be omitted if not needed
  "Barometric_Pressure",     // Client name that uniquely identify your device
  1883              // The MQTT port, default to 1883. this line can be omitted
);

void setup()
{
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
    if (!bmp.begin()) {
  Serial.println("Could not find a valid BMP085 sensor, check wiring!");
  while (1) {}
  }
  display.setRotation(2);
  display.clearDisplay();
  display.setTextSize(1.8);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(2,2);
  display.println("BOOTING UP");
  display.display();
  Serial.begin(115200);
  client.enableDebuggingMessages(); // Enable debugging messages sent to serial output
  client.enableLastWillMessage("sensi/demo/Barometric_Pressure_Sensor/events", "disconnected");
}

void onConnectionEstablished()
{
  client.subscribe("sensi/demo/Barometric_Pressure_Sensor", [](const String & payload) {
    Serial.println(payload);
  });
  client.publish("sensi/demo/Barometric_Pressure_Sensor/events", "connected");
  display.clearDisplay();
}


void loop()
{
  temp = bmp.readTemperature();
  pressure = bmp.readPressure();
  alt = bmp.readAltitude();

  client.loop();
  delay(2000);
  client.publish("sensi/demo/Barometric_Pressure_Sensor/telemetry/temperature", String(temp));
  client.publish("sensi/demo/Barometric_Pressure_Sensor/telemetry/pressure", String(pressure));
  client.publish("sensi/demo/Barometric_Pressure_Sensor/telemetry/altitude", String(alt));


  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(2,2);
  display.println(String(pressure));
  display.print(String(temp));
  display.print(",");
  display.println(String(alt));
  display.display();
  delay(200);
  
  display.clearDisplay();
}
