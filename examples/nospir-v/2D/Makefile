
CC=cc -Wall -Wextra
FLAGS=`pkgconf --cflags --libs lucurious`

all:
	$(CC) -g -ggdb $(FLAGS) simple_example.c -o se

clean:
	$(RM) se
