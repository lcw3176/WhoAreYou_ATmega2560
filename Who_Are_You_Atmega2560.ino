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

#define TXD 11
#define RXD 10
#define startFlag "<start>"
#define endFlag "<end>"
#define HOST "172.30.1.12"
#define PATH "/auth/socket"
#define SENSOR 18
// HIGH -> 분리, LOW -> 접촉
// requestCommand / data


enum BluetoothRequest{
  SetWiFiName,
  SetWiFiPassword,
  DisconnectWiFi,
  SetDeviceName,
  SetEmail,
  SetToken,
};

SoftwareSerial bluetooth(RXD, TXD);
String request;

String ssid;
String password;

String token;
String deviceName;
String email;

bool isReadyToConnect = false;

WiFiClient client;
WebSocketClient webSocketClient;

void setup() {
  bluetooth.begin(9600);
  Serial.begin(9600);
  
  attachInterrupt(SENSOR, DetachSensor, RISING);
  attachInterrupt(SENSOR, AttachSensor, FALLING);
}

void loop() {
  
  if(bluetooth.available())
  {
    request += bluetooth.readString();
    
    if(request.endsWith(endFlag))
    {
      String data = request.substring(0, data.indexOf(startFlag));
      AllocData(data);
    }
  }

//  if(isReadyToConnect)
//  {
//    if(TryConnectWiFi())
//    {
//      if(ConnectToServer())
//      {
//        isReadyToConnect = false;
//      }
//    }
//  }

}

boolean ConnectToServer()
{
  if(client.connect(HOST, 8080))
  {
    char* charToken = (char *)malloc(sizeof(char) * token.length());
    token.toCharArray(charToken, token.length());
    webSocketClient.path = PATH;
    webSocketClient.host = HOST;
    webSocketClient.token = charToken;
    
    if(webSocketClient.handshake(client))
    {
      free(charToken);
      return true;
    }
    
  }

  return false;
}

boolean TryConnectWiFi()
{
  WiFi.begin(ssid.c_str(), password.c_str());
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
    case SetDeviceName:
      deviceName = parameter;
      break;
    case SetEmail:
      email = parameter;
      break;
    case SetToken:
      token = parameter;
      break;
    default:
      break;
  }
  
  isReadyToConnect = (ssid != NULL) && (password != NULL) && (deviceName != NULL) && (email != NULL) && (token != NULL);
  request = "";
}

void AttachSensor()
{
  
}

void DetachSensor()
{
//  String msg = 
//  SendToServer()
}

boolean SendToServer(String data)
{
  if(client.connected())
  {
    webSocketClient.sendData(data);
    return true;  
  }
  
  return false;
}
