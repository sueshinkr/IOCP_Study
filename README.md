# IOCP 채팅 서버 제작

![111](https://user-images.githubusercontent.com/100945798/227977379-cb8c64c2-5d26-46db-950f-df60ccddee71.png)


[서버를 제작하는데 참고한 자료](https://github.com/jacking75/edu_cpp_IOCP)    
전반적인 서버의 구조 및 동작은 위 링크의 자료를 바탕으로 함

IOCP를 사용한 채팅 서버를 제작해보는 것이 목표    

![1](https://user-images.githubusercontent.com/100945798/227977529-c905d277-6716-42ab-9235-8f2eb605461c.gif)

![2](https://user-images.githubusercontent.com/100945798/227977578-9c72bacc-9668-48f1-b262-21be883ea9a6.gif)

*** 

## 현재 구현한 기능 목록    
* 서버 접속
* 유저 로그인 (Redis DB와 연동)
* 유저 로그아웃
* 채팅방 입장
* 채팅방 퇴장
* 채팅방 내부 채팅

***

## 서버 로직

1. 서버 실행시 IOCP 오브젝트 및 Listen소켓 연결
2. PacketManager / UserManager / RoomManager / RedisManager 객체 생성
3. 클라이언트 생성, Worker쓰레드 및 Accepter쓰레드 생성

Accepter쓰레드
* Accepter쓰레드에서는 생성해놓은 클라이언트들을 감시하면서 서버로의 연결 요청이 있는지 확인
* 연결 요청이 있을경우 비동기 I/O Accept 작업을 수행

Worker쓰레드
* Worker쓰레드에서는 CP에 연결되어있는 소켓들을 감시
* 완료된 IO를 감지했을 경우 Overlapped 구조체의 IOOperation 값에 따라 Accept / Recv / Send를 수행
	* Accept : 비동기 I/O Accept 작업 완료, 클라이언트 소켓을 CP에 등록하고 Recv를 요청
	* Recv : 비동기 I/O Recv 작업 완료, 수신한 패킷 데이터를 PacketManager로 넘겨 처리
	* Send : 비동기 I/O Send 작업 완료, SendQueue에서 송신한 데이터의 메모리를 해제하고 Queue가 비어있지 않을 경우 다시 Send 작업 수행

PacketManger
* PacketManager로 넘겨진 패킷 데이터는 Deque에 저장
* PacketManager의 ProcessPacket쓰레드에서 지속적으로 해당 Deque를 확인하여 패킷이 존재할경우 해당 패킷의 Id를 확인
* Id에 매칭되는 함수를 실행, 각 함수에서는 요청된 패킷의 데이터 크기를 체크하여 정상적인 패킷인지 확인
* 함수별 로직 실행 후의 결과를 패킷으로 만들어 Send

Redis
* 유저 로그인 / 로그아웃은 Redis에서 유저의 아이디를 Key값, 비밀번호를 Value값으로 하여 관리

