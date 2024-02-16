SRC = src/game.c src/assets.c src/particles.c src/seed_packets.c src/plant.c 

pvz: $(SRC)
	gcc $(SRC) -lraylib -lGL -lm -lpthread -o ./pvz
	./pvz

run:
	./pvz
