NAME 	= 	send
SRCS 	= 	observer.c sendManager.c
CC		=	gcc
OBJ		=	send.o sendManager.o
LIBS	=	-lasound  -lm

all:	$(NAME)

cc:
	rm $(NAME)
	$(CC) $(SRCS) $(LIBS) -o $(NAME)

pullcc:
	git pull
	rm  $(NAME)
	$(CC) $(SRCS) $(LIBS) -o $(NAME)

ccc:
	$(CC) $(SRCS) $(LIBS) -o $(NAME)

push:
	git add .
	git commit -m "test"
	git push

clean:
	rm -f *.o *~$(NAME)
