# FileTransferWithMFC
File and Chatting message transfer program with MFC

## 인원   

|이름 | E-mail | ID |
| ---| ---| ---|
| 박정현 | rkwaol216@gmail.com| BiteSnail|
| 남동욱 | egddbr1213@naver.com | egddbr1213 |
| 신용인 | brin7584@gmail.com | poeo1877 |
| 이석호 | choonarm3@gmail.com | choonarm3 |
| 최준희| junhee9807@gmail.com | serhong |
| 최재웅| jaewoongchoi46@gmail.com | sleepychoi |

## Dependency
`WinPcap` [Download](https://www.winpcap.org/)   
`MFC` <i>다운은 VisualStudio Installer에서 가능</i>

## 실습 목적
1. 송수신 프로토콜 스택 흐름을 이해한다.
2. 프로토콜 레이어별 역할을 파악한다.
3. 네트워크 어뎁터를 이용할 수 있다.

## 실습 시나리오
1. 두 대의 PC에서 각각 프로그램을 실행
2. 두 대의 PC는 네트워크로 연결
3. PC 1의 프로그램
    1. 전송할 파일 선택 or 1. 전송할 텍스트 입력
    2. 전송 버튼을 클릭
    3. 레이어 아키텍쳐에 의해서 Encapsulated Packet이 만들어짐 (Ethernet frame)
    4. NILayer의 Pcap 라이브러리로 작성된 코드에 의해 packet은 네트워크로 전송됨
4. PC 2의 프로그램에서 PC1으로부터 Ethernet frame을 수신
    1. 하위 레이어부터 수신된 프로토콜이 PC 1로부터 보내진 패킷인지 검사
        * 아니라면, discard(버림)
    2. 맞으면, 레이어 아키텍쳐에 의해서 Demultiplexing을 통해 header를 제외한 data 부분을 상위 레이어로 전달
        * Header는 매 레이어마다 receive 함수에서 frame에 대해 나에게 온 것이 맞는 검사할 때 쓰임
    3. 최상위 레이어 (*Dlg.cpp)에 전달 될 때 까지 1번과 2번 과정을 반복함
    4. 전달된 결과 확인
5. 파일인 경우, 채팅&파일전송 프로그램이 위치한 곳에 설치됨 (기본 저장 경로 사용 시)
6. 텍스트인 경우, 채팅&파일전송 프로그램 화면에서 채팅창(CListbox) 에 표기 됨
    + 표기 방법은 IPC에서 채팅 표기 방법과 동일

## 프로토콜 스택
프로그램을 이루고 있는 기본적인 구조와 역할에 대한 설명입니다.

### 구조 설명
![image](https://user-images.githubusercontent.com/55152597/194047333-52ef6a76-bc2e-4f76-bb82-819752c261c3.png)
> 출처 : woojin415@naver.com   

### 각 레이어의 역할
#### File & Chat Dlg
사용자로부터 Source Accepter, Destination Address 설정 및 전송할 파일 및 메시지 입력   

#### CChatAppLayer
File&Caht Dlg Layer에서 텍스트를 전송/수신하는 경우 메시지의 길이에 따라 단편화를 통해 전송할 수 있도록 함

#### CFileAppLayer
File&Caht Dlg Layer에서 파일을 전송/수신하는 경우 파일의 크기에 따라 단편화를 통해 전송할 수 있도록 함

#### Ethernet Layer
수신된 프레임의 목적지 확인 후 수용 혹은 파기   

#### Network Interface Layer
Network Adapter와 상위 Layer간의 데이터 송수신을 중재함   
