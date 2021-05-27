#include <SoftwareSerial.h>

#define BLERXD 10
#define BLETXD 11

#define WIFIRXD 12
#define WIFITXD 13

#define startFlag "<start>"
#define endFlag "<end>"
#define SENSOR 18
// HIGH -> 분리, LOW -> 접촉

SoftwareSerial bluetoothSerial(BLERXD, BLETXD);
SoftwareSerial wifiSerial(WIFIRXD, WIFITXD);
String request;

// 데이터 전달 형식: <start> / requestCommand / data / <end>
// Ex: <start>/0/KT_2G_GIGA_WIFI/<end>
enum RequestCommand{
  SetWiFiName,
  SetWiFiPassword,
  DisconnectWiFi,
  SetDeviceName,
  SetEmail,
  SetToken,
  SensorChanged,
};

void setup() {
  bluetoothSerial.begin(9600);
  wifiSerial.begin(9600);
  Serial.begin(9600);
  attachInterrupt(SENSOR, DetachSensor, RISING);
  attachInterrupt(SENSOR, AttachSensor, FALLING);
}

void loop() {
  bluetoothSerial.listen();
  
  if(bluetoothSerial.available())
  {
    request += (char)bluetoothSerial.read();

    if(request.endsWith(endFlag))
    {
      request = request.substring(request.indexOf(startFlag), request.length());
      request.remove(0, 8); // <start> 제거
      Serial.println(request);
      
      wifiSerial.write(request.c_str());
      request = "";
    }
  }

}

// 센서 접촉됬을 때
void AttachSensor()
{
  
}

// 센서 분리됬을 때
void DetachSensor()
{

}
