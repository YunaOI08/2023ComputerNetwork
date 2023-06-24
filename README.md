# 2023ComputerNetwork
4학년 1학기 컴퓨터네트워크 수업을 위한 레포지토리입니다.

## 목적
Multi-threading 기반 채팅프로그램

## 기능
- 1:1 대화 기능 구현 (@대화명을 통해 사용자 지정)
- 대화방 생성/입장/초대/강퇴 기능
- 접속한 클라이언트 간 상호작용을 통해 진행 가능한 게임 (ex.스무고개, 빙고, 야구게임


## 실행 방법
cd 2023ComputerNetwork/src
1. Thread Creation
```bash
# thread2.c
gcc thread1.c -o ./.o/trl -lpthread
./tr1
# thread2.c
gcc mutex/thread2.c -o ./.o/tr2 -lpthread
./tr1
# mutex
gcc mutex/mutex.c -o ./.o/mutex -lpthead
./mutex
```
2. Chat
```bash
gcc chat_serv.c -o ./.o/chat_serv -lpthread
./chat_serv 7777

gcc chat_clnt.c -o ./.o/chat_clnt -lpthread
./chat_clnt 127.0.0.1 7777 {ID} # ID에 원하는 대화명 입력
```

## Reference
[1] [YU_컴퓨터네트워크및실습] Lab7.Multi-threading-based Server and Client
