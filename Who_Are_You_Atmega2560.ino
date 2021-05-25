#include <SoftwareSerial.h>

#define BLETXD 11
#define BLERXD 10
#define WIFITXD 13
#define WIFIRXD 12
#define startFlag "<start>"
#define endFlag "<end>"
#define SENSOR 18
// HIGH -> 분리, LOW -> 접촉

SoftwareSerial bluetoothSerial(BLERXD, BLETXD);
SoftwareSerial wifiSerial(WIFIRXD, WIFITXD);
String request;

void setup() {
  bluetoothSerial.begin(9600);
  wifiSerial.begin(9600);
  Serial.begin(9600);
  
  attachInterrupt(SENSOR, DetachSensor, RISING);
  attachInterrupt(SENSOR, AttachSensor, FALLING);
}

void loop() {
  
  if(bluetoothSerial.available())
  {
    char recvData = bluetoothSerial.read();
    request += recvData;
    
    if(request.endsWith(endFlag))
    {
      String data = request.substring(request.indexOf(startFlag), request.length());
      data.remove(0, 8); // <start> 제거
      wifiSerial.write(data.c_str());
    }
  }

}

void AttachSensor()
{
  
}

void DetachSensor()
{

}
