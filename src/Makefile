CC=gcc
OBJS_DIR=./.o/
RM = rm -f

############################################################
all : tr1 tr2 mutex chat_clnt chat_serv
thread : tr1 tr2 mutex
chat : chat_clnt chat_serv

clean : 
	$(RM) $(OBJS_DIR)*
############################################################

tr1 : thread1.c
	${CC} thread1.c -o $(OBJS_DIR)trl -lpthread
tr2 : thread2.c
	${CC} thread2.c -o $(OBJS_DIR)tr2 -lpthread
mutex: mutex.c
	${CC} mutex.c -o $(OBJS_DIR)mutex -lpthread

chat_clnt : chat_clnt.c
	${CC} chat_clnt.c -o $(OBJS_DIR)chat_clnt -lpthread
chat_serv : chat_serv.c
	${CC} chat_serv.c -o $(OBJS_DIR)chat_serv -lpthread