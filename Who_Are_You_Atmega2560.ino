#include <SoftwareSerial.h>
#include <util/delay.h>

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
  Serial.begin(9600);
  bluetoothSerial.begin(9600);
  wifiSerial.begin(9600);
  pinMode(SENSOR, INPUT_PULLUP);
  attachInterrupt(5, SensorEvent, CHANGE);
}

void loop() {
  bluetoothSerial.listen();
  
  if(bluetoothSerial.available())
  {
    request += (char)bluetoothSerial.read();

    if(request.endsWith(endFlag))
    {
      request = request.substring(request.indexOf(startFlag), request.length());
      Serial.println(request);
      wifiSerial.write(request.c_str());
      request = "";
    }
  }

}

volatile int state = 0;

void SensorEvent()
{
  _delay_ms(500);
  int temp = digitalRead(SENSOR);
  _delay_ms(500);
  
  if(state != temp)
  {
    state = temp;
    if(state == HIGH)
    {
      Serial.println("분리");
      String params = "<start>/6/false/<end>";
      wifiSerial.write(params.c_str());
    }
  
    else
    {
      Serial.println("접촉");
      String params = "<start>/6/true/<end>";
      wifiSerial.write(params.c_str());
    }
  
  }
  
  
}
