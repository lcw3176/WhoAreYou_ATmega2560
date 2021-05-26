#include <WebSocketClient.h>
#include <ESP8266WiFi.h>
#include <SoftwareSerial.h>

#define TXD 1
#define RXD 3
#define startFlag "<start>"
#define endFlag "<end>"
#define HOST "172.30.1.12"
#define PATH "/auth/socket"

// <start> / requestCommand / data / <end>
enum BluetoothRequest{
  SetWiFiName,
  SetWiFiPassword,
  DisconnectWiFi,
  SetDeviceName,
  SetEmail,
  SetToken,
};


WiFiClient wifiClient;
WebSocketClient webSocketClient;
SoftwareSerial wifiSerial(RXD, TXD);

String ssid = "";
String password = "";

String token = "";
String deviceName = "";
String email = "";
String request = "";
bool isReadyToConnect = false;

void setup() {
  wifiSerial.begin(9600);
  pinMode(0, OUTPUT); // 디버깅 포트
  digitalWrite(0, LOW);
}

void loop() {
  
  if(wifiSerial.available())
  {    
    request += (char)wifiSerial.read();

    if(request.length() > 7 && !request.startsWith(startFlag))
    {
      request = "";
    }

    if(request.endsWith(endFlag))
    {
      digitalWrite(0, HIGH);
      request = request.substring(request.indexOf(startFlag), request.length());
      request.remove(0, 8); // <start> 제거
      AllocData(request);
      request = "";
    }
  }

  if(isReadyToConnect)
  {
        digitalWrite(0, LOW);
        
    if(ConnectWiFi())
    {
        digitalWrite(0, LOW);
        delay(2000);
        digitalWrite(0, HIGH);
        delay(2000);
        
      if(ConnectToServer())
      {
        digitalWrite(0, LOW);
        delay(1000);
        digitalWrite(0, HIGH);
        delay(1000);
        digitalWrite(0, LOW);
        delay(1000);
        digitalWrite(0, HIGH);
        delay(1000);
        SendToServer(email + "," + deviceName + ",true");
      }
    }

    isReadyToConnect = false;
  }
}

void AllocData(String data)
{
  int command = data.substring(0, 1).toInt();
  String parameter = data.substring(2, data.indexOf(endFlag) - 1);

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
      isReadyToConnect = true;
      break;
    default:
      break;
  }
  
}


boolean ConnectToServer()
{
  if(wifiClient.connect(HOST, 8080))
  {
    char charToken[150];
    strcpy(charToken, token.c_str());
    webSocketClient.path = PATH;
    webSocketClient.host = HOST;
    webSocketClient.token = charToken;
    
    if(webSocketClient.handshake(wifiClient))
    {
      return true;
    }
   
  }

  return false;
}


boolean ConnectWiFi()
{
  char id[64]; 
  char pw[64]; 
  strcpy(id, ssid.c_str());
  strcpy(pw, password.c_str());

  WiFi.begin(id, pw);
  bool st = true;
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    if(st){
      digitalWrite(0, LOW);
    }

    else{
       digitalWrite(0, HIGH);
    }

    st = !st;
  }

  return true;
}


boolean SendToServer(String data)
{
  if(wifiClient.connected())
  {
    webSocketClient.sendData(data);
    return true;  
  }
  
  return false;
}
