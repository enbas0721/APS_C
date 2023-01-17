NAME 	= 	estimate
SRCS 	= 	estimator.c recordManager.c trackManager.c WavManager/audioio.c thermo.c
CC		=	gcc
OBJ		=	estimator.o recordManager.o trackManager.o WavManager/audioio.o thermo.o
LIBS	=	-lasound -lpthread

all:	$(NAME)

cc:
	rm $(NAME)
	$(CC) $(SRCS) $(LIBS) -o $(NAME)

clean:
	rm -f *.o *~$(NAME)
