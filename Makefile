SRC = src/game.c src/assets.c src/particles.c src/seed_packets.c src/plant.c src/zombie.c src/sun.c
HEADERS = src/game.h src/assets.h src/particles.h src/seed_packets.h src/plant.h src/zombie.h src/sun.h

pvz: $(SRC) $(HEADERS)
	gcc -g $(SRC) -lraylib -lGL -lm -lpthread -o ./pvz
	./pvz

run:
	./pvz
