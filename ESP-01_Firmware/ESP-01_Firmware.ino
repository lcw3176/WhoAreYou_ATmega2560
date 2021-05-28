#include <WebSocketClient.h>
#include <ESP8266WiFi.h>
#include <SoftwareSerial.h>

#include <Thread.h>

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

Thread serialThread = Thread();
Thread connectionThread = Thread();

String ssid;
String password;

String token;
String deviceName;
String email;
String request;
bool isReadyToConnect;

void setup() {
  Init();
  wifiSerial.begin(9600);
  pinMode(2, OUTPUT);
  digitalWrite(2, LOW);
  
  serialThread.onRun(AllocData);
  connectionThread.onRun(TryConnectServer);
}

void loop() {
  
  if(wifiSerial.available())
  {    
    digitalWrite(2, HIGH);
    request += (char)wifiSerial.read();
        
    if(request.endsWith(endFlag))
    {
      serialThread.run();
    }
    
  }

  if(isReadyToConnect)
  {
    connectionThread.run();
  }
}

void TryConnectServer()
{
  digitalWrite(2, LOW);
  if(ConnectWiFi())
  {
    digitalWrite(2, HIGH);
    if(ConnectToServer())
    {
      digitalWrite(2, LOW);
      isReadyToConnect = false;
    }

    else
    {
      Init();  
    }
  }

  else
  {
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

void AllocData()
{
  int command = request.substring(0, 1).toInt();
  String parameter = request.substring(2, request.indexOf(endFlag) - 1);

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

   request = "";
}


bool ConnectToServer()
{
  if(wifiClient.connect(HOST, 8080))
  {
    delay(300);
    char* charToken = (char*)malloc(sizeof(char) * token.length());
//    char charToken[150];
    strcpy(charToken, token.c_str());
    webSocketClient.path = PATH;
    webSocketClient.host = HOST;
    webSocketClient.token = charToken;
    
    if(webSocketClient.handshake(wifiClient))
    {
      delay(300);
      free(charToken);
      return true;
    }

   free(charToken);
  }
 
  return false;
}


bool ConnectWiFi()
{
  char* charId = (char*)malloc(sizeof(char) * ssid.length()); 
  char* charPw = (char*)malloc(sizeof(char) * password.length()); 

//  char charId[30];
//  char charPw[30];
  strcpy(charId, ssid.c_str());
  strcpy(charPw, password.c_str());

  WiFi.begin(charId, charPw);
  unsigned char count = 0;
  bool st = true;
  
  while (WiFi.status() != WL_CONNECTED) {
    count++;
    delay(1000);
    digitalWrite(2, st);
    st = !st;
    if(count >= 30)
    {
      return false;
    }
  }

  free(charId);
  free(charPw);
  
  return true;
}


bool SendToServer(String state)
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
