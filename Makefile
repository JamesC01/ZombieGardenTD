pvz: game.c
	gcc game.c -lraylib -lGL -lm -lpthread -o pvz
	./pvz

run:
	./pvz
