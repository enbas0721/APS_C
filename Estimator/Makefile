NAME 	= 	estimate.a
SRCS 	= 	estimator.c recordManager.c trackManager.c WavManager/audioio.c
CC		=	gcc
LIB		=	-lasound -lpthread

.c.o:
	$(CC) -c $< -o $@

clean:
	rm -f $(NAME)

re:
	fclean all