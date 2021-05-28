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
- 21.05.27
    * 의도대로 작동이 너무 안되서 포럼 글들 찾아봤더니 loop문만 계속 돈다는 글을 봤다.
    * 나도 나름대로 실험을 해봤는데 코드를 새로 올릴 때 빼고는 setup 문이 작동을 안했다
    * 그래서 확실하진 않지만 loop 기준으로 데이터 초기화, 분배를 다시 작성하니 잘 작동한다.
- 21.05.28
    * 윗글은 틀린 정보같다. 또 작동이 안된다.
    * 에러코드: Exception(0), rst cause 2, 4
    * 더 찾아봤더니 Core Logic이 작동할 시간을 안주거나 사용자 코드가 길게 돌아간다고 판단하면 watchDogTimer란 놈이 리셋을 시킨댄다.
    * 이걸 막기 위해선 loop문 안에서 사용자 함수가 50ms 이상 시간을 쓰는걸 권장하지 않는다고 한다.
    * 그래서 쓰레드 라이브러리 가져다가 썼다. 이제 되면 좋겠다.