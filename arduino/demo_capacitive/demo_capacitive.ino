#include "EspMQTTClient.h"

EspMQTTClient client(
  "Vinci's Workshop",
  "asdfzxcv",
  "159.203.2.152",  // MQTT Broker server ip
  "sensi",   // Can be omitted if not needed
  "password",   // Can be omitted if not needed
  "Capacitive_touch",     // Client name that uniquely identify your device
  1883              // The MQTT port, default to 1883. this line can be omitted
);

struct Button {
  const uint8_t PIN;
  uint32_t numberKeyPresses;
  bool pressed;
};
Button button1 = {33, 0, false};

void IRAM_ATTR isr() {
  button1.numberKeyPresses++;
  button1.pressed = true;
}



void setup()
{
  Serial.begin(115200);
  client.enableDebuggingMessages(); // Enable debugging messages sent to serial output
  client.enableLastWillMessage("sensi/demo/Capacitive_Touch_Sensor/events", "disconnected");
  pinMode(button1.PIN, INPUT_PULLUP);
  pinMode(2, OUTPUT);
  attachInterrupt(button1.PIN, isr, FALLING);
}

void onConnectionEstablished()
{
  client.subscribe("sensi/demo/Capacitive_Touch_Sensor", [](const String & payload) {
    Serial.println(payload);
  });
  client.publish("sensi/demo/Capacitive_Touch_Sensor/events", "connected");
}


void loop()
{
  client.loop();
  if(button1.pressed){
  client.publish("sensi/demo/Capacitive_Touch_Sensor/telemetry/switch", String(button1.numberKeyPresses));
  Serial.printf("Button has been pressed %u times\n", button1.numberKeyPresses);
  button1.pressed = false;
  }
  if(button1.numberKeyPresses % 2 == 0){digitalWrite(2,LOW);}
  else{digitalWrite(2,HIGH);}
}
