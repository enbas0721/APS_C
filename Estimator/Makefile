NAME 	= 	estimate
SRCS 	= 	estimator.c recordManager.c trackManager.c WavManager/audioio.c
CC		=	gcc
OBJ		=	estimator.o recordManager.o trackManager.o WavManager/audioio.o
LIBS	=	-lasound -lpthread

all:	$(NAME)

$(NAME):	$(OBJS)
	$(CC) $(OBJS) $(LIBS) -o $(NAME)

clean:
	rm -f *.o *~$(NAME)