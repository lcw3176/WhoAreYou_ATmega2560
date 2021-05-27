#include <WebSocketClient.h>
#include <ESP8266WiFi.h>
#include <SoftwareSerial.h>

#define TXD 1
#define RXD 3
#define startFlag "<start>"
#define endFlag "<end>"
#define HOST "172.30.1.42"
#define PATH "/auth/socket"

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


WiFiClient wifiClient;
WebSocketClient webSocketClient;
SoftwareSerial wifiSerial(RXD, TXD);

String ssid;
String password;

String token;
String deviceName;
String email;
String request;
bool isReadyToConnect;

// Reset, boot x
void setup() {
  Init();
  wifiSerial.begin(9600);
  pinMode(0, OUTPUT); // 디버깅 포트
  digitalWrite(0, LOW);
}

void loop() {
  
  if(wifiSerial.available())
  {    
    request += (char)wifiSerial.read();

    if(request.endsWith(endFlag))
    {
      digitalWrite(0, HIGH);
      AllocData(request);
      request = "";
    }
  }

  if(isReadyToConnect)
  {
    digitalWrite(0, LOW);       
    if(ConnectWiFi())
    {
      digitalWrite(0, HIGH);
      if(ConnectToServer())
      {
        digitalWrite(0, LOW);
        SendToServer("true");
      }
    }

    Init();
  }
}

void Init()
{
  ssid = "";
  password = "";
  token = "";
  deviceName = "";
  email = "";
  request = "";
  isReadyToConnect = false;
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
    case SensorChanged:
      SendToServer(parameter);
      break;
    default:
      break;
  }
  
}


boolean ConnectToServer()
{
  if(wifiClient.connect(HOST, 8080))
  {
//    char* charToken = (char*)malloc(sizeof(char) * token.length());
    char charToken[150];
    strcpy(charToken, token.c_str());
    webSocketClient.path = PATH;
    webSocketClient.host = HOST;
    webSocketClient.token = charToken;
    
    if(webSocketClient.handshake(wifiClient))
    {
//      free(charToken);
      return true;
    }

//   free(charToken);
  }
 
  return false;
}


boolean ConnectWiFi()
{
//  char* charId = (char*)malloc(sizeof(char) * ssid.length()); 
//  char* charPw = (char*)malloc(sizeof(char) * password.length()); 
  if(WiFi.status() == WL_CONNECTED)
  {
    WiFi.disconnect(true);  
  }
  char charId[30];
  char charPw[30];
  strcpy(charId, ssid.c_str());
  strcpy(charPw, password.c_str());
  
  WiFi.begin(charId, charPw);
  bool st = true;
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    digitalWrite(0, st);
    st = !st;

  }

//  free(charId);
//  free(charPw);
  
  return true;
}


boolean SendToServer(String state)
{
  // state
  // true -> 분리됨
  // false -> 접촉됨

  if(wifiClient.connected())
  {
    webSocketClient.sendData(email + "," + deviceName + "," + state);
    
    return true;  
  }
  
  return false;
}
