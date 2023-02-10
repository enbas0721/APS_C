NAME 	= 	estimate
SRCS 	= 	estimator.c recordManager.c trackManager.c WavManager/audioio.c thermo.c
CC		=	gcc
OBJ		=	estimator.o recordManager.o trackManager.o WavManager/audioio.o thermo.o
LIBS	=	-lasound -lpthread -lm

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
