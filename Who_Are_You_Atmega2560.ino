#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiServer.h>
#include <WiFiUdp.h>

#include <Base64.h>
#include <global.h>
#include <MD5.h>
#include <sha1.h>
#include <WebSocketClient.h>
#include <WebSocketServer.h>

#include <SoftwareSerial.h>

#define TXD 14
#define RXD 15
#define EOL "<end>"
#define SERVER "172.30.1.24"
#define SENSOR 18
// HIGH -> 분리, LOW -> 접촉
// requestCommand / data / <end>

// 헤더 설정 어떻게하지
enum BluetoothRequest{
  SetWiFiName,
  SetWiFiPassword,
  DisconnectWiFi,
  SetDeviceName,
  SetEmail,
};

SoftwareSerial bluetooth(RXD, TXD);
String request;
String ssid = NULL;
String password = NULL;
bool isReadyToConnect = false;

WiFiClient client;
WebSocketClient webSocketClient;

void setup() {
  bluetooth.begin(9600);
  attachInterrupt(SENSOR, DetachSensor, RISING);
  attachInterrupt(SENSOR, AttachSensor, FALLING)
}

void loop() {
  
  if(bluetooth.available())
  {
    request += bluetooth.read();
   
    if(request.endsWith(EOL))
    {
      AllocData(request);
    }
  }

  if(isReadyToConnect)
  {
    if(TryConnectWiFi())
    {
      if(ConnectToServer())
      {
        isReadyToConnect = false;
      }
    }
  }

}

boolean ConnectToServer()
{
  if(client.connect(SERVER, 8080))
  {
    webSocketClient.handshake(client);
    
    return true;
  }

  return false;
}

boolean TryConnectWiFi()
{
  WiFi.begin(ssid, password);
  int timeoutCount = 0;
  
  while(WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    timeoutCount += 1;

    if(timeoutCount >= 10)
    {
      return false;
    }
  }

  return true;
}

void AllocData(String data)
{
  int command = data.substring(0, 1).toInt();
  String parameter = data.substring(2, data.indexOf("/", 2)); 

  switch(command)
  {
    case SetWiFiName:
      ssid = parameter;
      break;
    case SetWiFiPassword:
      password = parameter;
      break;
    case DisconnectWiFi:
      ssid = "";
      password = "";
      break;
    default:
      break;
  }

  isReadyToConnect = (ssid != NULL) && (password != NULL);
  request = "";
}

void AttachSensor()
{
  
}

void DetachSensor()
{
  String msg = 
  SendToServer()
}

boolean SendToServer(String data)
{
  if(client.connected()
  {
    webSocketClient.sendData(data);
    return true;  
  }
  
  return false;
}