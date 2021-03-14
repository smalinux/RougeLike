headers=  -lncurses -lpanel -lform -lmenu

all:
		gcc -o RogueLike RougeLike.c $(headers) -std=c99
		#gcc -o RL-V1.o ./RL-V1.c $(headers) -std=c99

run:
	./RogueLike

clean:
	rm *.o
