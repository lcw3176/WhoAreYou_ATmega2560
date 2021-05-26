#include <SoftwareSerial.h>

#define BLERXD 10
#define BLETXD 11

#define WIFIRXD 12
#define WIFITXD 13

#define endFlag "<end>"
#define SENSOR 18
// HIGH -> 분리, LOW -> 접촉

SoftwareSerial bluetoothSerial(BLERXD, BLETXD);
SoftwareSerial wifiSerial(WIFIRXD, WIFITXD);
String request;

enum WiFiResult{
  ConnectionWiFiError,
  ConnectionServerError,
};


void setup() {
  bluetoothSerial.begin(9600);
  wifiSerial.begin(9600);
  Serial.begin(9600);
//  attachInterrupt(SENSOR, DetachSensor, RISING);
//  attachInterrupt(SENSOR, AttachSensor, FALLING);
}

void loop() {
  bluetoothSerial.listen();
  
  if(bluetoothSerial.available())
  {
    request += (char)bluetoothSerial.read();

    if(request.endsWith(endFlag))
    {
      Serial.println(request);

      wifiSerial.write(request.c_str());
      request = "";
    }
  }

}

void AttachSensor()
{
  
}

void DetachSensor()
{

}
