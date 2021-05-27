# WhoAreYou_ATmega2560
마이크로프로세서 수업 학기말 프로젝트 임베디드

### 개발환경
* ATmega 2560 Arduino
* C++, Sketch
* Websocket, SoftwareSerial

### 사용부품
* ESP8266 ESP-01 와이파이 모듈
* HC-06 블루투스 센서
* MC-38 마그네틱 센서

### 참고 회로도
![회로](https://user-images.githubusercontent.com/59993347/119628243-e2aee400-be47-11eb-9224-3b6e06e33225.png)
* 출처: https://rasino.tistory.com/299, 라즈이노 IOT

### 기록사항
#### ESP-01 와이파이 모듈
* 의도대로 작동이 너무 안되서 포럼 글들 찾아봤더니 loop문만 계속 돈다는 글을 봤다.
* 나도 나름대로 실험을 해봤는데 코드를 새로 올릴 때 빼고는 setup 문이 작동을 안했다.
* 그래서 확실하진 않지만 loop 기준으로 데이터 초기화, 분배를 다시 작성하니 잘 작동한다.
