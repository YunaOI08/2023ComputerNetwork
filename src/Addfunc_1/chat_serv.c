#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>

#define BUF_SIZE 100
#define MAX_CLNT 256

void * handle_clnt(void * arg); // 클라이언트 연결을 처리하는 함수의 프로토 타입
void send_msg(char * msg, int len); // 모든 클라이언트에게 메시지를 보내는 함수의 프로토 타입
void error_handling(char * msg); // 오류 처리를 위한 함수의 프로토 타입

int clnt_cnt=0; // 연결된 클라이언트 수
int clnt_socks[MAX_CLNT]; // 클라이언트 소켓 디스크립터를 저장하는 배열
pthread_mutex_t mutx;  // 스레드 동기화를 위한 뮤텍스

int main(int argc, char *argv[])
{
	int serv_sock, clnt_sock;
	struct sockaddr_in serv_adr, clnt_adr;
	int clnt_adr_sz;
	pthread_t t_id;
	if(argc!=2) {
		printf("Usage : %s <port>\n", argv[0]);
		exit(1);
	}
  
	pthread_mutex_init(&mutx, NULL); // mutex 초기화
	serv_sock=socket(PF_INET, SOCK_STREAM, 0); //  socket 생성

	memset(&serv_adr, 0, sizeof(serv_adr)); //  서버 주소 구조체 초기화
	serv_adr.sin_family=AF_INET;  // IPv4
	serv_adr.sin_addr.s_addr=htonl(INADDR_ANY); // 모든 IP 주소로부터의 연결을 허용
	serv_adr.sin_port=htons(atoi(argv[1])); // 포트 번호 설정
	
	if(bind(serv_sock, (struct sockaddr*) &serv_adr, sizeof(serv_adr))==-1) // socket을 서버 주소에 바인딩
		error_handling("bind() error");
	if(listen(serv_sock, 5)==-1) // 클라이언트 연결을 위해 대기
		error_handling("listen() error");
	
	while(1)
	{
		clnt_adr_sz=sizeof(clnt_adr);
		clnt_sock=accept(serv_sock, (struct sockaddr*)&clnt_adr,&clnt_adr_sz); // 클라이언트 연결 수락
		
		pthread_mutex_lock(&mutx); // 공유 데이터에 접근하기 전에 mutex 잠금
		clnt_socks[clnt_cnt++]=clnt_sock; // 클라이언트의 socket descriptor를 배열에 추가
		pthread_mutex_unlock(&mutx); // mutex 잠금 해제
	
		pthread_create(&t_id, NULL, handle_clnt, (void*)&clnt_sock); // 클라이언트 연결을 처리하기 위해 새 thread 생성
		pthread_detach(t_id); // thread 분리 (독립적으로 종료 가능)
		printf("Connected client IP: %s \n", inet_ntoa(clnt_adr.sin_addr)); // 연결된 클라이언트의 IP 주소 출력
	}
	close(serv_sock); // 서버 소켓 닫기
	return 0;
}
	
void * handle_clnt(void * arg)
{
	int clnt_sock=*((int*)arg); // arg로부터 클라이언트 socket descriptor 추출
	int str_len=0, i;
	char msg[BUF_SIZE];
	
	while((str_len=read(clnt_sock, msg, sizeof(msg)))!=0) // 클라이언트로부터 메세지 읽기
		send_msg(msg, str_len); // 모든 연결된 클라이언트에게 메세지 전송
	
	pthread_mutex_lock(&mutx); // 공유 데이터에 접근하기 전에 mutex 잠금
	for(i=0; i<clnt_cnt; i++)   // 연결이 끊긴 클라이언트 제거
	{
		if(clnt_sock==clnt_socks[i])
		{
			while(i <clnt_cnt-1)
			{
				clnt_socks[i]=clnt_socks[i+1];
				  i++;

			}

			break;
		}
	}
	clnt_cnt--;
	pthread_mutex_unlock(&mutx);
	close(clnt_sock);
	return NULL;
}
void send_msg(char * msg, int len)   // 모든 클라이언트에게 메세지 전송
{
	int i;
	pthread_mutex_lock(&mutx); // 공유 데이터에 접근하기 전에 mutex 잠금
	for(i=0; i<clnt_cnt; i++)
		write(clnt_socks[i], msg, len);
	pthread_mutex_unlock(&mutx); // mutex 잠금 해제
}
void error_handling(char * msg)
{
	fputs(msg, stderr);
	fputc('\n', stderr);
	exit(1);
}
