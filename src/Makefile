CC=gcc
OBJS_DIR=./.o/
RM = rm -f

THR_DIR=./thread/
CHAT_DIR=./Addfunc_1/
############################################################
all : tr1 tr2 mutex chat_clnt chat_serv
thread : tr1 tr2 mutex
chat : chat_clnt chat_serv

clean : 
	$(RM) $(OBJS_DIR)*
############################################################

tr1 : $(THR_DIR)thread1.c
	${CC} $(THR_DIR)thread1.c -o $(OBJS_DIR)tr1 -lpthread
tr2 : $(THR_DIR)thread2.c
	${CC} $(THR_DIR)thread2.c -o $(OBJS_DIR)tr2 -lpthread
mutex: $(THR_DIR)mutex.c
	${CC} $(THR_DIR)mutex.c -o $(OBJS_DIR)mutex -lpthread

chat_clnt : $(CHAT_DIR)chat_clnt.c
	${CC} $(CHAT_DIR)chat_clnt.c -o $(OBJS_DIR)chat_clnt -lpthread
chat_serv : $(CHAT_DIR)chat_serv.c
	${CC} $(CHAT_DIR)chat_serv.c -o $(OBJS_DIR)chat_serv -lpthread


