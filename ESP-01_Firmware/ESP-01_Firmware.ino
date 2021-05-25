#include <WiFi.h>
#include <WebSocketClient.h>

#include <SoftwareSerial.h>

#define TXD 1
#define RXD 3
#define endFlag "<end>"
#define HOST "172.30.1.12"
#define PATH "/auth/socket"

// requestCommand / data / <end>
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

String ssid;
String password;

String token;
String deviceName;
String email;
String request;
bool isReadyToConnect = false;

void setup() {
  wifiSerial.begin(9600);
}

void loop() {
  
  if(wifiSerial.available())
  {
    char recvData = wifiSerial.read();
    request += recvData;

    if(request.endsWith(endFlag))
    {
      Serial.println("할당: " + request);
      AllocData(request);
      request = "";
    }
  }

  if(isReadyToConnect)
  {
    Serial.println("커넥트 시작");
    if(ConnectWiFi())
    {
      if(ConnectToServer())
      {
        SendToServer(email + "," + deviceName + ",1");
        isReadyToConnect = false;
        return;
      }
    }

    Serial.println("failed");
    isReadyToConnect = false;
  }
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
}


boolean ConnectToServer()
{
  if(wifiClient.connect(HOST, 8080))
  {
    char* charToken = (char *)malloc(sizeof(char) * token.length());
    token.toCharArray(charToken, token.length());
    webSocketClient.path = PATH;
    webSocketClient.host = HOST;
    webSocketClient.token = charToken;
    
    if(webSocketClient.handshake(wifiClient))
    {
      free(charToken);
      return true;
    }
    
    free(charToken);
  }

  return false;
}


boolean ConnectWiFi()
{
  Serial.println("와이파이 비긴");
  WiFi.begin(const_cast<char*>(ssid.c_str()),const_cast<char*>(password.c_str()));
  int timeoutCount = 0;
  
  while(WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    timeoutCount += 1;
    Serial.println("연결 대기중");
    if(timeoutCount >= 10)
    {
      return false;
    }
  }

  Serial.println("연결 성공");

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
