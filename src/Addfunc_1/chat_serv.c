#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>

#define BUF_SIZE 100
#define NAME_SIZE 20
#define MAX_CLNT 256

typedef struct {
	int clnt_sock; // client socket descriptor
	char clnt_name[NAME_SIZE]; // Client name
} ClientInfo;

void * handle_clnt(void * arg);
void send_msg(char * msg, int len, char *sender_name, char *dest_name);
void error_handling(char * msg);

int clnt_cnt=0;
ClientInfo clnt_socks[MAX_CLNT]; // client information 
pthread_mutex_t mutx;

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
	pthread_mutex_init(&mutx, NULL);
	serv_sock=socket(PF_INET, SOCK_STREAM, 0);

	memset(&serv_adr, 0, sizeof(serv_adr));
	serv_adr.sin_family=AF_INET; 
	serv_adr.sin_addr.s_addr=htonl(INADDR_ANY);
	serv_adr.sin_port=htons(atoi(argv[1]));
	
	if(bind(serv_sock, (struct sockaddr*) &serv_adr, sizeof(serv_adr))==-1)
		error_handling("bind() error");
	if(listen(serv_sock, 5)==-1)
		error_handling("listen() error");
	
	while(1)
	{
		clnt_adr_sz=sizeof(clnt_adr);
		clnt_sock=accept(serv_sock, (struct sockaddr*)&clnt_adr,&clnt_adr_sz);

		pthread_mutex_lock(&mutx);
		clnt_socks[clnt_cnt].clnt_sock=clnt_sock; // add new client
		// sprintf(clnt_socks[clnt_cnt].clnt_name, "[%s:%d]", inet_ntoa(clnt_adr.sin_addr), ntohs(clnt_adr.sin_port));
		read(clnt_sock, clnt_socks[clnt_cnt].clnt_name, sizeof(clnt_socks[clnt_cnt].clnt_name));
		printf("Connected client IP: %s, Name: %s\n", inet_ntoa(clnt_adr.sin_addr), clnt_socks[clnt_cnt].clnt_name);
		clnt_cnt++;
		pthread_mutex_unlock(&mutx);
		
		pthread_create(&t_id, NULL, handle_clnt, (void*)&clnt_sock);
		pthread_detach(t_id);
	}
	close(serv_sock);
	return 0;
}
	
void * handle_clnt(void * arg)
{
	int clnt_sock=*((int*)arg);
	int str_len=0, i;
	char msg[BUF_SIZE];
	char sender_name[NAME_SIZE];
	char dest_name[NAME_SIZE];
	
	pthread_mutex_lock(&mutx);
	//find client name
	for (i=0;i<clnt_cnt;i++) {
		if (clnt_sock==clnt_socks[i].clnt_sock) {
			strcpy(sender_name, clnt_socks[i].clnt_name);
			//printf("sender's name is %s\n", sender_name);
			break;
		}
	}
	pthread_mutex_unlock(&mutx);
	
	while((str_len=read(clnt_sock, msg, sizeof(msg)))!=0) {
		sscanf(msg, "%s", dest_name);
		//sscanf(msg, "%s %s", dest_name, msg);
		//printf("msg : %s\n", msg);
		//printf("sender_name :  %s\n", sender_name);		
		//sprintf(msg, "[%s] %s", sender_nam+'\0', msg);
		//printf("msg :  %s\n", msg);
		if (strcmp(dest_name, "all") == 0) {
			send_msg(msg+strlen(dest_name)+1, str_len-strlen(dest_name)-1, sender_name, NULL);
		}
		else {
			send_msg(msg+strlen(dest_name)+1, str_len-strlen(dest_name)-1, sender_name, dest_name);
		}
		memset(msg, 0, sizeof(msg));
	}
	pthread_mutex_lock(&mutx);
	
	// remove disconnected client
	for(i=0; i<clnt_cnt; i++)
	{
		if(clnt_sock==clnt_socks[i].clnt_sock) {
			while(i <clnt_cnt-1) {
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
void send_msg(char * msg, int len, char *sender_name, char *dest_name)   // send to all
{
	int i;
	pthread_mutex_lock(&mutx);
	//sprintf(msg,"[%s] %s", sender_name, msg);
	printf("%s send message to %s\t| %s\n", sender_name, dest_name, msg);
	if (dest_name==NULL) {
		for (i=0;i<clnt_cnt;i++) {
			if (strcmp(clnt_socks[i].clnt_name, sender_name)==0) {
				write(clnt_socks[i].clnt_sock, "there is no client", strlen("there is no client"));
				break;
	} else {
		for (i=0;i<clnt_cnt;i++) {
			if (strcmp(clnt_socks[i].clnt_name, dest_name)==0) {
				//write(clnt_socks[i].clnt_sock, sender_name, NAME_SIZE);
				//write(clnt_socks[i].clnt_sock, ":", 2);
				//write(clnt_socks[i].clnt_sock, msg, BUF_SIZE);
				write(clnt_socks[i].clnt_sock, msg, strlen(msg));
				break;
			}
		}
	}
	pthread_mutex_unlock(&mutx);
}
void error_handling(char * msg)
{
	fputs(msg, stderr);
	fputc('\n', stderr);
	exit(1);
}
